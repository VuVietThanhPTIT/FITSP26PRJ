#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace std;

struct Symbol {
    char ch;
    int freq;
    string code;
};
// Mô tả chương trình:
// Chương trình này cài đặt giải thuật Shannon-Fano để mã hóa ký tự
// dựa trên tần suất xuất hiện trong chuỗi vào. Output bao gồm
// bảng mã (ký tự, tần suất, mã nhị phân), chuỗi mã hóa và chuỗi giải mã.
// Lưu ý: mã được xây dựng theo cách quy ước '0' cho nửa đầu, '1' cho nửa sau.

// Hàm sắp xếp tần suất giảm dần
bool compareSymbols(Symbol a, Symbol b) {
    if (a.freq != b.freq)
        return a.freq > b.freq;
    return a.ch < b.ch; // Sắp xếp thêm theo ký tự nếu tần suất bằng nhau để mã hóa ổn định
}

// Thuật toán chia nhóm đệ quy Shannon-Fano
void generateShannonFano(int start, int end, vector<Symbol>& symbols) {
    if (start >= end) return;

    if (start + 1 == end) {
        // Nếu chỉ còn hai phần tử, gán trực tiếp 0 và 1
        symbols[start].code += '0';
        symbols[end].code += '1';
        return;
    }

    int total = 0;
    for (int i = start; i <= end; i++) total += symbols[i].freq;

    int half = 0;
    int diff = total;
    int splitPoint = start;

    for (int i = start; i < end; i++) {
        half += symbols[i].freq;
        if (abs((total - half) - half) < diff) {
            diff = abs((total - half) - half);
            splitPoint = i;
        }
    }

    // Gán bit '0' cho phần nửa đầu, '1' cho nửa sau theo splitPoint
    for (int i = start; i <= splitPoint; i++) symbols[i].code += '0';
    for (int i = splitPoint + 1; i <= end; i++) symbols[i].code += '1';

    generateShannonFano(start, splitPoint, symbols);
    generateShannonFano(splitPoint + 1, end, symbols);
}

int main(int argc, char* argv[]) {
    string input;

    if (argc > 1) {
        std::ifstream fin(argv[1]);
        if (fin) {
            std::ostringstream ss;
            ss << fin.rdbuf();
            input = ss.str();
        } else {
            cerr << "Khong mo duoc file: " << argv[1] << "\nSẽ chuyển sang nhập tay.\n";
        }
    }

    if (input.empty()) {
        cout << "Nhap chuoi can ma hoa: ";
        getline(cin, input);
    }

    if (input.empty()) return 0;

    map<char, int> counts;
    for (int i = 0; i < input.length(); i++) counts[input[i]]++;

    vector<Symbol> symbols;
    for (map<char, int>::iterator it = counts.begin(); it != counts.end(); ++it) {
        Symbol s;
        s.ch = it->first;
        s.freq = it->second;
        s.code = "";
        symbols.push_back(s);
    }

    sort(symbols.begin(), symbols.end(), compareSymbols);

    // XU LY TRUONG HOP DAC BIET: Chỉ có 1 loại ký tự
    if (symbols.size() == 1) {
        // Nếu chỉ có 1 ký tự xuất hiện trong chuỗi, gán mã 0 cho ký tự đó
        symbols[0].code = "0";
    } else {
        generateShannonFano(0, symbols.size() - 1, symbols);
    }

    // --- CÁC PHẦN CÒN LẠI GIỮ NGUYÊN ---
    // In bảng mã: ký tự | tần suất | mã nhị phân
    cout << "\n--- BANG MA SHANNON-FANO ---\n";
    cout << left << setw(10) << "Ky tu" << setw(10) << "Tan suat" << "Ma nhi phan" << endl;
    for (int i = 0; i < symbols.size(); i++) {
        string display = (symbols[i].ch == ' ') ? "Space" : string(1, symbols[i].ch);
        cout << setw(10) << display << setw(10) << symbols[i].freq << symbols[i].code << endl;
    }

    string encoded = "";
    map<char, string> encoderMap;
    for (int i = 0; i < symbols.size(); i++) encoderMap[symbols[i].ch] = symbols[i].code;
    for (int i = 0; i < input.length(); i++) encoded += encoderMap[input[i]];

    cout << "\nChuoi sau khi ma hoa: " << encoded << endl;

    string decoded = "";
    string temp = "";
    for (int i = 0; i < encoded.length(); i++) {
        temp += encoded[i];
        for (int j = 0; j < symbols.size(); j++) {
            if (symbols[j].code == temp) {
                decoded += symbols[j].ch;
                temp = "";
                break;
            }
        }
    }

    cout << "Chuoi sau khi giai ma: " << decoded << endl;
    cout << "\nKiem tra tinh toan ven: " << (input == decoded ? "THANH CONG" : "THAT BAI") << endl;

    return 0;
}
