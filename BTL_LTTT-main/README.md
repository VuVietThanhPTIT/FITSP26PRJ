# README Tong Hop Du An

## 1) Thong tin chung
- Ten du an: Cac thuat toan ma hoa nguon (Huffman, Shannon-Fano, Shannon-Fano-Elias).
- Muc tieu: Minh hoa va danh gia hieu qua ma hoa dua tren tan suat/xac suat ky hieu.
- Ngon ngu chinh:
  - C++: Cai dat thuat toan va chuong trinh dong lenh.
  - Python + Flask: Giao dien web de chay cac thuat toan.
- Link git du an: https://github.com/VuVietThanhPTIT/BTL_LTTT

## 2) Cau truc codebase
```
BTL_LTTT/
  CMakeLists.txt
  README.md
  README_FINAL.md
  cpp/
    main.cpp                  # Shannon-Fano-Elias (SFE)
  examples/
    1..6                      # Du lieu mau cho SFE
  huffman/
    Huffman.cpp
    README.md
    input*.txt, output*.txt
  shannonfano/
    ShannonFano.cpp
    README.md
    input*.txt, output*.txt
  web/
    app.py                    # Flask server + compile/chay executable
    README.md
    templates/index.html
    static/main.js
    static/styles.css
  web_bin/
    *.exe                     # Sinh ra sau khi chay web/compile
```

## 3) Thiet ke tong quan
He thong gom 2 tang:

1. Tang xu ly thuat toan (C++)
- Huffman:
  - Xay cay Huffman tu bang tan suat ky tu.
  - Sinh bang ma nhi phan, ma hoa va giai ma.
  - Co thong ke kich thuoc goc/nen va ti le nen.
- Shannon-Fano:
  - Sap xep ky tu theo tan suat giam dan.
  - Chia nhom de quy toi uu can bang tong tan suat.
  - Sinh ma, ma hoa, giai ma va kiem tra toan ven.
- Shannon-Fano-Elias:
  - Tinh F(x), Fbar(x), do dai ma theo cong thuc.
  - Sinh codeword, kiem tra prefix code.
  - Tinh entropy, average length, efficiency.

2. Tang trien khai/chay thu (Flask Web)
- API tra danh sach thuat toan.
- Tu dong compile C++ thanh executable (thu muc web_bin/).
- Ho tro 3 che do input:
  - Nhap text truc tiep.
  - Upload file.
  - Chon sample.
- Tra ket qua stdout/stderr ve giao dien web.

## 4) Yeu cau moi truong
- Windows 10/11 (khuyen nghi) hoac Linux/macOS.
- Trinh bien dich C++: g++ (MinGW tren Windows).
- Python 3.8+.
- Pip package: Flask.
- Git de clone source.

Kiem tra nhanh:
```bash
g++ --version
python --version
```

## 5) Cai dat va chuan bi
### 5.1 Clone du an
```bash
git clone https://github.com/VuVietThanhPTIT/BTL_LTTT.git
cd BTL_LTTT
```

### 5.2 Cai Flask cho web
```bash
cd web
python -m pip install flask
cd ..
```

## 6) Huong dan chay chuong trinh (kiem tra tinh dung dan)

### 6.1 Chay Huffman (dong lenh)
```bash
cd huffman
g++ -std=c++11 Huffman.cpp -o Huffman.exe
./Huffman.exe
# hoac
./Huffman.exe input1.txt
```

Ket qua can co:
- Bang ma Huffman.
- Chuoi sau ma hoa.
- Chuoi sau giai ma trung voi input.
- Thong ke hieu suat nen.

### 6.2 Chay Shannon-Fano (dong lenh)
```bash
cd shannonfano
g++ -std=c++11 ShannonFano.cpp -o ShannonFano.exe
./ShannonFano.exe
# hoac
./ShannonFano.exe input1.txt
```

Ket qua can co:
- Bang ma Shannon-Fano.
- Chuoi ma hoa.
- Chuoi giai ma.
- Dong kiem tra tinh toan ven = THANH CONG.

### 6.3 Chay Shannon-Fano-Elias (dong lenh)
Tu thu muc goc du an:
```bash
g++ -std=c++17 cpp/main.cpp -o shannon_fano_elias_cpp.exe
./shannon_fano_elias_cpp.exe -t
# hoac
./shannon_fano_elias_cpp.exe
```

Ket qua can co:
- Bang ky hieu/xac suat.
- Bang SFE (F(x), Fbar(x), codeword).
- Ket luan prefix code hop le.
- Chi so entropy, average length, efficiency.

### 6.4 Chay giao dien web
```bash
cd web
python app.py
```
Mo trinh duyet: http://127.0.0.1:5000

Luu y quan trong ve cau truc thu muc:
- Trong ban repo hien tai, source that su nam o:
  - huffman/Huffman.cpp
  - shannonfano/ShannonFano.cpp
  - cpp/main.cpp
- Neu web/app.py dang tro den cac duong dan cu (BTL-LTTT, Code Part 2a, shannon-fano-elias/cpp/main.cpp), hay cap nhat lai bien ALGS cho khop 3 duong dan tren.

Quy trinh test tren web:
1. Chon algorithm.
2. Chon input mode (text/file/sample).
3. Bam Run.
4. Xem stdout/stderr va ma tra ve.

## 7) Kiem thu va tieu chi danh gia tinh dung dan
1. Dung dan ma hoa/giai ma:
- Huffman/Shannon-Fano: chuoi sau giai ma phai trung chuoi goc.

2. Tinh chat ma:
- SFE: chuong trinh bao valid prefix code.

3. Do on dinh dau vao:
- Thu voi chuoi ngan, chuoi dai, ky tu lap lai, co khoang trang.
- Thu voi input file va input tu ban phim.

4. Chi so hieu qua:
- So sanh kich thuoc bit goc va sau nen (Huffman).
- Theo doi entropy/average length/efficiency (SFE).

