from flask import Flask, request, render_template, jsonify
import os
import subprocess
import tempfile
import sys
import shlex
import time

app = Flask(__name__, static_folder='static', template_folder='templates')

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
BIN_DIR = os.path.join(ROOT, 'web_bin')
if not os.path.exists(BIN_DIR):
    os.makedirs(BIN_DIR)

# Mapping algorithm id to source path and compile command
ALGS = {
    'huffman': {
        'src': os.path.join(ROOT, 'huffman', 'Huffman.cpp'),
        'exe': os.path.join(BIN_DIR, 'huffman'),
        'compile': 'g++ -std=c++11 "{src}" -O2 -o "{exe}"'
    },
    'shannonfano': {
        'src': os.path.join(ROOT, 'shannon-fano', 'ShannonFano.cpp'),
        'exe': os.path.join(BIN_DIR, 'shannonfano'),
        'compile': 'g++ -std=c++11 "{src}" -O2 -o "{exe}"'
    },
    'sfe': {
        'src': os.path.join(ROOT, 'shannon-fano-elias', 'shannon-fano-elias.cpp'),
        'exe': os.path.join(BIN_DIR, 'sfe'),
        'compile': 'g++ -std=c++11 "{src}" -O2 -o "{exe}"'
    }
}


def ensure_compiled(alg_key):
    info = ALGS[alg_key]
    exe = info['exe']
    if sys.platform.startswith('win'):
        exe += '.exe'
    if os.path.exists(exe):
        return exe
    cmd = info['compile'].format(src=info['src'], exe=exe)
    proc = subprocess.run(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    if proc.returncode != 0:
        raise RuntimeError(f"Compile failed:\n{proc.stdout}\n{proc.stderr}")
    return exe


@app.route('/')
def index():
    return render_template('index.html')


@app.route('/algorithms')
def algorithms():
    return jsonify([{'id': k, 'name': k} for k in ALGS.keys()])


@app.route('/samples/<alg>')
def samples(alg):
    # Look for example/sample files under known example dirs
    samples = []
    if alg == 'sfe':
        exdir = os.path.join(ROOT, 'shannon-fano-elias', 'examples')
        if os.path.exists(exdir):
            for name in sorted(os.listdir(exdir)):
                samples.append(name)
    elif alg == 'huffman':
        exdir = os.path.join(ROOT, 'huffman')
        if os.path.exists(exdir):
            for name in sorted(os.listdir(exdir)):
                if name.lower().startswith('input') or name.lower().endswith('.txt'):
                    samples.append(os.path.join(exdir, name))
    elif alg == 'shannonfano':
        exdir = os.path.join(ROOT, 'shannon-fano')
        if os.path.exists(exdir):
            for name in sorted(os.listdir(exdir)):
                if name.lower().startswith('input') or name.lower().endswith('.txt'):
                    samples.append(os.path.join(exdir, name))
    return jsonify(samples)


@app.route('/run', methods=['POST'])
def run():
    alg = request.form.get('algorithm')
    input_mode = request.form.get('inputMode')  # 'text' | 'file' | 'sample'
    text = request.form.get('text', '')
    sample = request.form.get('sample', '')

    if alg not in ALGS:
        return jsonify({'error': 'Unknown algorithm'}), 400

    try:
        exe = ensure_compiled(alg)
    except Exception as e:
        return jsonify({'error': str(e)}), 500

    stdin_data = None
    args = [exe]

    # handle input modes
    if input_mode == 'file':
        if 'file' not in request.files:
            return jsonify({'error': 'No file uploaded'}), 400
        f = request.files['file']
        # If algorithm expects filename as arg, provide it; sfe reads stdin so send content
        if alg == 'sfe':
            stdin_data = f.read().decode(errors='ignore')
        else:
            tmp = tempfile.NamedTemporaryFile(delete=False)
            f.save(tmp.name)
            tmp.close()
            # if uploaded file is empty, return error to avoid blocking programs that prompt for input
            if os.path.getsize(tmp.name) == 0:
                return jsonify({'error': 'Uploaded file is empty'}), 400
            args.append(tmp.name)
    elif input_mode == 'sample' and sample:
        # sample may be a simple name (for sfe) or a full path
        if alg == 'sfe':
            sample_path = os.path.join(ROOT, 'shannon-fano-elias', 'examples', sample)
            try:
                with open(sample_path, 'r', encoding='utf-8') as fh:
                    stdin_data = fh.read()
            except Exception:
                return jsonify({'error': 'Sample file not found'}), 400
        else:
            sample_path = sample
            # check sample exists and is non-empty
            if not os.path.exists(sample_path):
                return jsonify({'error': 'Sample file not found'}), 400
            if os.path.getsize(sample_path) == 0:
                return jsonify({'error': 'Sample file is empty'}), 400
            args.append(sample_path)
    else:  # text
        stdin_data = text

    # Run the executable
    try:
        t0 = time.time()
        proc = subprocess.run(args, input=stdin_data, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, timeout=30)
        t1 = time.time()
        print(f"Ran: {args} return={proc.returncode} duration={t1-t0:.3f}s")
        out = proc.stdout
        err = proc.stderr
        rc = proc.returncode
    except subprocess.TimeoutExpired:
        return jsonify({'error': 'Execution timed out'}), 500

    result = {'returncode': rc, 'stdout': out, 'stderr': err}
    return jsonify(result)


@app.route('/run_pipeline', methods=['POST'])
def run_pipeline():
    """Run a pipeline of executables, piping stdout of each to stdin of the next.
    Expects form fields:
      - pipeline: comma-separated algorithm keys (e.g. "huffman,sfe")
      - text: initial stdin text (optional)
    """
    pipeline = request.form.get('pipeline', '')
    initial_text = request.form.get('text', '')
    if not pipeline:
        return jsonify({'error': 'No pipeline specified'}), 400

    keys = [p.strip() for p in pipeline.split(',') if p.strip()]
    for k in keys:
        if k not in ALGS:
            return jsonify({'error': f'Unknown algorithm: {k}'}), 400

    try:
        exes = [ensure_compiled(k) for k in keys]
    except Exception as e:
        return jsonify({'error': str(e)}), 500

    current_input = initial_text
    steps = []
    for exe in exes:
        args = [exe]
        try:
            t0 = time.time()
            proc = subprocess.run(args, input=current_input, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, timeout=30)
            t1 = time.time()
            print(f"Pipeline step: {exe} return={proc.returncode} duration={t1-t0:.3f}s")
        except subprocess.TimeoutExpired:
            return jsonify({'error': 'Execution timed out'}), 500

        steps.append({'exe': exe, 'returncode': proc.returncode, 'stdout': proc.stdout, 'stderr': proc.stderr})
        if proc.returncode != 0:
            # stop pipeline on error
            break
        current_input = proc.stdout

    return jsonify({'steps': steps, 'final_stdout': current_input})


if __name__ == '__main__':
    app.run(debug=True, port=5000)
