# Shannon-Fano (Code Part 2a)

Mô tả:
- File chính: `ShannonFano.cpp`
- Cài đặt thuật toán Shannon-Fano để mã hóa ký tự dựa trên tần suất xuất hiện.

Chức năng chính:
- Xây bảng tần suất ký tự từ chuỗi vào.
- Chia nhóm đệ quy để sinh mã nhị phân (gán '0' cho nửa đầu, '1' cho nửa sau).
- In bảng mã, mã hóa chuỗi, giải mã và kiểm tra tính toàn vẹn.

Cách biên dịch và chạy (Windows, MinGW/g++):
```bash
g++ -std=c++11 "ShannonFano.cpp" -o ShannonFano.exe
./ShannonFano.exe
```
Bạn có thể truyền file làm input (đọc toàn bộ nội dung file):
```bash
./ShannonFano.exe input.txt
```

Ghi chú:
- Nếu chỉ có một ký tự trong chuỗi, chương trình gán mã "0" cho ký tự đó.
- File `ShannonFano.cpp` đã được chú thích bằng tiếng Việt để giải thích luồng xử lý và các trường hợp đặc biệt.
