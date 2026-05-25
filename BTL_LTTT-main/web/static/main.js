document.addEventListener('DOMContentLoaded', () => {
  const algorithm = document.getElementById('algorithm');
  const inputMode = document.getElementById('inputMode');
  const textArea = document.getElementById('textArea');
  const fileArea = document.getElementById('fileArea');
  const sampleArea = document.getElementById('sampleArea');
  const sampleSelect = document.getElementById('sample');
  const form = document.getElementById('runForm');
  const output = document.getElementById('output');

  function updateInputAreas() {
    const mode = inputMode.value;
    textArea.style.display = (mode === 'text') ? 'block' : 'none';
    fileArea.style.display = (mode === 'file') ? 'block' : 'none';
    sampleArea.style.display = (mode === 'sample') ? 'block' : 'none';
  }

  inputMode.addEventListener('change', updateInputAreas);

  algorithm.addEventListener('change', () => {
    // load samples for algorithm
    fetch('/samples/' + algorithm.value)
      .then(r => r.json())
      .then(list => {
        sampleSelect.innerHTML = '';
        list.forEach(item => {
          const opt = document.createElement('option');
          opt.value = item;
          opt.textContent = item;
          sampleSelect.appendChild(opt);
        });
      });
  });

  // initial load
  updateInputAreas();
  algorithm.dispatchEvent(new Event('change'));

  form.addEventListener('submit', (e) => {
    e.preventDefault();
    output.textContent = 'Running...';

    const formData = new FormData();
    formData.append('algorithm', algorithm.value);
    formData.append('inputMode', inputMode.value);
    formData.append('text', document.getElementById('text').value);
    formData.append('sample', sampleSelect.value);

    if (inputMode.value === 'file') {
      const f = document.getElementById('file').files[0];
      if (f) formData.append('file', f);
    }

    fetch('/run', { method: 'POST', body: formData })
      .then(r => r.json())
      .then(res => {
        if (res.error) {
          output.textContent = 'Error:\n' + res.error;
        } else {
          output.textContent = 'Return code: ' + res.returncode + '\n\nSTDOUT:\n' + res.stdout + '\n\nSTDERR:\n' + res.stderr;
        }
      }).catch(err => {
        output.textContent = 'Request failed: ' + err;
      });
  });
});