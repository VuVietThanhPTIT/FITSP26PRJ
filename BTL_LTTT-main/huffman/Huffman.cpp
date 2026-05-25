#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace std;

// ============================================================
// CẤU TRÚC NÚT CÂY HUFFMAN
// Mỗi nút lưu: ký tự (ch), tần suất xuất hiện (freq),
// con trỏ đến nút con trái và nút con phải
// ============================================================
struct Node {
    char ch;        // Ký tự được lưu (chỉ có ý nghĩa ở nút lá)
    int freq;       // Tần suất xuất hiện của ký tự (hoặc tổng tần suất của cây con)
    Node *left, *right; // Con trỏ đến nút con trái và phải

    // Hàm khởi tạo nút
    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}

    // Hàm hủy: tự động giải phóng toàn bộ cây con (đệ quy)
    ~Node() {
        delete left;
        delete right;
    }
};

// ============================================================
// HÀM SO SÁNH CHO HÀNG ĐỢI ƯU TIÊN (MIN-HEAP)
// Nút có tần suất NHỎ HƠN sẽ được ưu tiên lấy ra trước
// => priority_queue mặc định là max-heap nên phải đảo dấu
// ============================================================
struct Compare {
    bool operator()(Node* l, Node* r) {
        return l->freq > r->freq; // Đảo chiều: nút freq nhỏ hơn lên trước
    }
};

// ============================================================
// LỚP HUFFMAN: chứa toàn bộ logic mã hóa và giải mã
// ============================================================
class Huffman {
private:
    Node* root;                     // Gốc của cây Huffman
    map<char, string> huffmanCode;  // Bảng ánh xạ: ký tự -> mã nhị phân

    // ----------------------------------------------------------
    // Hàm đệ quy sinh mã Huffman cho từng ký tự
    // - Đi sang trái: thêm '0' vào chuỗi mã
    // - Đi sang phải: thêm '1' vào chuỗi mã
    // - Khi đến nút lá: lưu mã vào bảng huffmanCode
    // Trường hợp đặc biệt: cây chỉ có 1 nút (1 loại ký tự) => gán mã "0"
    // ----------------------------------------------------------
    void generateCodes(Node* node, string str) {
        if (!node) return; // Nút rỗng thì bỏ qua

        // Nếu là nút lá (không có con) => đây là ký tự cần mã hóa
        if (!node->left && !node->right) {
            // Nếu cây chỉ có 1 nút (str rỗng), gán mã mặc định là "0"
            huffmanCode[node->ch] = (str == "") ? "0" : str;
        }

        generateCodes(node->left,  str + "0"); // Rẽ trái thêm bit 0
        generateCodes(node->right, str + "1"); // Rẽ phải thêm bit 1
    }

public:
    // Khởi tạo: chưa có cây, bảng mã rỗng
    Huffman() : root(nullptr) {}

    // Hủy: giải phóng toàn bộ cây Huffman
    ~Huffman() { delete root; }

    // ----------------------------------------------------------
    // XÂY DỰNG CÂY HUFFMAN TỪ VĂN BẢN ĐẦU VÀO
    // Thuật toán:
    //   1. Đếm tần suất từng ký tự
    //   2. Tạo nút lá cho mỗi ký tự, đẩy vào min-heap
    //   3. Lặp: lấy 2 nút nhỏ nhất, gộp thành nút cha, đẩy lại heap
    //   4. Khi heap còn 1 phần tử => đó là gốc cây
    // ----------------------------------------------------------
    void buildTree(string text) {
        if (text.empty()) return; // Không xử lý chuỗi rỗng

        // Bước 1: Đếm tần suất xuất hiện của mỗi ký tự
        map<char, int> freq;
        for (char ch : text) freq[ch]++;

        // Bước 2: Tạo min-heap từ các nút lá
        priority_queue<Node*, vector<Node*>, Compare> pq;
        for (auto pair : freq)
            pq.push(new Node(pair.first, pair.second));

        // Bước 3: Gộp dần các nút có tần suất nhỏ nhất thành cây
        while (pq.size() > 1) {
            Node *left  = pq.top(); pq.pop(); // Nút tần suất nhỏ nhất -> nhánh trái
            Node *right = pq.top(); pq.pop(); // Nút tần suất nhỏ thứ hai -> nhánh phải

            // Tạo nút cha với tần suất = tổng 2 nút con
            Node *parent = new Node('\0', left->freq + right->freq);
            parent->left  = left;
            parent->right = right;
            pq.push(parent); // Đẩy nút cha trở lại heap
        }

        // Bước 4: Nút cuối cùng trong heap là gốc cây
        root = pq.top();

        // Sinh mã Huffman cho tất cả ký tự dựa trên cây vừa xây
        huffmanCode.clear();
        generateCodes(root, "");
    }

    // ----------------------------------------------------------
    // MÃ HÓA: Chuyển văn bản thành chuỗi bit nhị phân
    // Mỗi ký tự được thay bằng mã Huffman tương ứng trong bảng
    // ----------------------------------------------------------
    string encode(string text) {
        string res = "";
        for (char ch : text)
            res += huffmanCode[ch]; // Tra bảng và ghép mã
        return res;
    }

    // ----------------------------------------------------------
    // GIẢI MÃ: Chuyển chuỗi bit nhị phân trở về văn bản gốc
    // Duyệt từng bit, đi theo cây:
    //   - bit '0' -> rẽ trái
    //   - bit '1' -> rẽ phải
    //   - Đến nút lá -> ghi ký tự, quay về gốc
    // Trường hợp đặc biệt: cây chỉ có 1 nút (1 loại ký tự)
    //   -> mỗi bit '0' tương ứng 1 lần xuất hiện ký tự đó
    // ----------------------------------------------------------
    string decode(string encodedStr) {
        if (!root) return ""; // Chưa có cây thì không giải mã được

        string res = "";

        // Trường hợp đặc biệt: cây chỉ có 1 nút gốc (1 loại ký tự duy nhất)
        if (!root->left && !root->right) {
            for (char bit : encodedStr) {
                if (bit == '0') res += root->ch; // Mỗi '0' = 1 ký tự
            }
            return res;
        }

        // Trường hợp bình thường: duyệt cây theo từng bit
        Node* curr = root; // Bắt đầu từ gốc
        for (char bit : encodedStr) {
            // Rẽ trái nếu bit = '0', rẽ phải nếu bit = '1'
            curr = (bit == '0') ? curr->left : curr->right;

            // Nếu đến nút lá: đã giải mã được 1 ký tự
            if (!curr->left && !curr->right) {
                res += curr->ch; // Ghi ký tự vào kết quả
                curr = root;     // Quay về gốc để giải mã ký tự tiếp theo
            }
        }
        return res;
    }

    // ----------------------------------------------------------
    // IN BẢNG MÃ HUFFMAN RA MÀN HÌNH
    // ----------------------------------------------------------
    void printCodes() {
        cout << left << setw(10) << "Ky tu" << "Ma Huffman" << endl;
        cout << "------------------------" << endl;
        for (const auto& kv : huffmanCode) {
            // Hiển thị "(space)" thay cho ký tự khoảng trắng cho dễ đọc
            char ch = kv.first;
            const string& code = kv.second;
            string label = (ch == ' ') ? "(space)" : string(1, ch);
            cout << left << setw(10) << label << code << endl;
        }
    }
};

// ============================================================
// HÀM MAIN: Chạy chương trình minh họa mã hóa Huffman
// ============================================================
int main(int argc, char* argv[]) {
    Huffman hf;
    string input;

    // Nếu truyền tên file qua tham số dòng lệnh, đọc toàn bộ file làm input
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

    // Nếu không có input từ file, yêu cầu nhập tay từ người dùng
    if (input.empty()) {
        cout << "--- MINH HOA MA HOA HUFFMAN ---\n";
        cout << "Nhap van ban: ";
        getline(cin, input); // Đọc cả dòng (kể cả khoảng trắng)

        // Kiểm tra đầu vào rỗng
        if (input.empty()) {
            cout << "Loi: Khong co du lieu." << endl;
            return 0;
        }
    }

    // Xây dựng cây Huffman và sinh bảng mã
    hf.buildTree(input);

    // 1. In bảng mã đã tạo
    cout << "\n1. BANG MA DA DUOC TAO:\n";
    hf.printCodes();

    // 2. Mã hóa văn bản gốc thành chuỗi bit
    string encoded = hf.encode(input);
    cout << "\n2. CHUOI SAU KHI MA HOA:\n" << encoded << endl;

    // 3. Giải mã lại để kiểm tra tính đúng đắn
    string decoded = hf.decode(encoded);
    cout << "\n3. GIAI MA DE KIEM TRA:\n" << "\"" << decoded << "\"" << endl;

    // 4. Tính toán hiệu suất nén
    // Văn bản gốc: mỗi ký tự = 8 bit (ASCII chuẩn)
    // Văn bản nén: số bit = độ dài chuỗi mã hóa
    double originalBits   = input.length() * 8; // Kích thước gốc (bits)
    double compressedBits = encoded.length();    // Kích thước sau nén (bits)
    double ratio = (compressedBits / originalBits) * 100; // Tỉ lệ % so với bản gốc

    cout << "\n4. PHAN TICH HIEU SUAT:\n";
    cout << "- Kich thuoc goc:       " << originalBits   << " bits\n";
    cout << "- Kich thuoc sau nen:   " << compressedBits << " bits\n";
    cout << "- Ty le nen:            " << fixed << setprecision(2) << ratio << "%"
         << "  (" << (100.0 - ratio) << "% tiet kiem)\n";

    return 0;
}