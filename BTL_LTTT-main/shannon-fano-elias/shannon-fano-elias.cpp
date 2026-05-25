#include <algorithm>
#include <chrono>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using SymbolProb = std::pair<std::string, double>;

std::string binaryCode(double x, std::size_t bits) {
    x = std::round(x * 1e8) / 1e8;
    if (x < 0.0 || x > 1.0) {
        throw std::runtime_error("Modified cumulative frequency out of range");
    }

    if (x == 1.0) {
        return std::string(bits, '0');
    }

    std::string code;
    code.reserve(bits);
    for (std::size_t i = 1; i <= bits; ++i) {
        const double threshold = std::pow(2.0, -static_cast<double>(i));
        if (x >= threshold) {
            x -= threshold;
            code.push_back('1');
        } else {
            code.push_back('0');
        }
    }
    return code;
}

std::map<std::string, std::string> sfe(const std::vector<SymbolProb>& prob) {
    std::map<std::string, std::string> result;
    double cumulative = 0.0;

    for (const auto& [symbol, probability] : prob) {
        const double mcf = cumulative + probability / 2.0;
        const std::size_t length = static_cast<std::size_t>(std::ceil(-std::log2(probability))) + 1;
        result[symbol] = binaryCode(mcf, length);
        cumulative += probability;
    }

    return result;
}

struct TrieNode {
    std::unique_ptr<TrieNode> child[2];
    bool endOfWord = false;
};

class Trie {
public:
    bool insert(const std::string& key) {
        TrieNode* node = &root_;
        for (char ch : key) {
            if (node->endOfWord) {
                return true;
            }

            const int index = ch == '0' ? 0 : 1;
            if (!node->child[index]) {
                node->child[index] = std::make_unique<TrieNode>();
            }
            node = node->child[index].get();
        }

        if (node->endOfWord) {
            return true;
        }

        node->endOfWord = true;
        return false;
    }

private:
    TrieNode root_;
};

bool isPrefixCode(const std::map<std::string, std::string>& codes) {
    Trie trie;
    for (const auto& [symbol, code] : codes) {
        (void)symbol;
        if (trie.insert(code)) {
            return false;
        }
    }
    return true;
}

double entropy(const std::vector<SymbolProb>& prob) {
    double result = 0.0;
    for (const auto& [symbol, p] : prob) {
        (void)symbol;
        if (p > 0.0) {
            result += -p * std::log2(p);
        }
    }
    return result;
}

double averageLength(const std::map<std::string, std::string>& code, const std::vector<SymbolProb>& prob) {
    double result = 0.0;
    for (const auto& [symbol, p] : prob) {
        result += p * static_cast<double>(code.at(symbol).size());
    }
    return result;
}

double efficiency(double ent, double avgLen) {
    return ent / avgLen;
}

int randomExampleIndex() {
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now).count();
    return static_cast<int>(seconds % 6) + 1;
}

std::vector<SymbolProb> loadExample() {
    const int index = randomExampleIndex();
    const std::string path = "examples/" + std::to_string(index);
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Failed to open example file: " + path);
    }

    std::cout << "Loading Test Data : " << path << '\n';

    int count = 0;
    input >> count;
    if (!input) {
        throw std::runtime_error("Invalid example file: " + path);
    }

    std::vector<SymbolProb> prob;
    prob.reserve(static_cast<std::size_t>(count));
    for (int i = 0; i < count; ++i) {
        double value = 0.0;
        input >> value;
        if (!input) {
            throw std::runtime_error("Invalid probability data in: " + path);
        }
        prob.emplace_back("x" + std::to_string(i), value);
    }

    return prob;
}

std::vector<SymbolProb> readCustomInput() {
    int count = 0;
    std::cout << "Please enter the number of symbols : ";
    std::cin >> count;

    std::cout << "\nPlease enter the respective probabilities :\n";
    std::vector<SymbolProb> prob;
    prob.reserve(static_cast<std::size_t>(count));
    for (int i = 0; i < count; ++i) {
        double value = 0.0;
        const std::string symbol = "x" + std::to_string(i);
        std::cout << ' ' << symbol << "   -> ";
        std::cin >> value;
        prob.emplace_back(symbol, value);
    }

    std::cout << '\n';
    return prob;
}

void printInputTable(const std::vector<SymbolProb>& prob) {
    std::cout << "\n# Symbols\n\n";
    std::cout << std::left << std::setw(10) << "Symbol" << std::setw(12) << "Probability" << '\n';
    std::cout << "----------------------\n";
    for (const auto& [symbol, p] : prob) {
        std::cout << std::left << std::setw(10) << symbol
                  << std::fixed << std::setprecision(4)
                  << std::setw(12) << p << '\n';
    }
    std::cout << '\n';
}

void printCodingTable(const std::vector<SymbolProb>& prob, const std::map<std::string, std::string>& codes) {
    std::cout << "# Shannon-Fano-Elias-Coding\n";
    std::cout << '\n';
    std::cout << std::left
              << std::setw(10) << "Symbol"
              << std::setw(14) << "Probability"
              << std::setw(12) << "F(x)"
              << std::setw(12) << "Fbar(x)"
              << std::setw(16) << "Codeword" << '\n';
    std::cout << "---------------------------------------------------------------\n";

    double cumulative = 0.0;
    for (const auto& [symbol, p] : prob) {
        cumulative += p;
        const double modified = cumulative - p / 2.0;
        std::cout << std::left
                  << std::setw(10) << symbol
                  << std::setw(14) << std::fixed << std::setprecision(4) << p
                  << std::setw(12) << cumulative
                  << std::setw(12) << modified
                  << std::setw(16) << codes.at(symbol) << '\n';
    }
}

int main(int argc, char* argv[]) {
    try {
        std::vector<SymbolProb> prob;
        if (argc > 1 && std::string(argv[1]) == "-t") {
            prob = loadExample();
        } else {
            prob = readCustomInput();
        }

        printInputTable(prob);

        const auto codes = sfe(prob);
        printCodingTable(prob, codes);

        std::cout << '\n';
        std::cout << "---------------------------------------------------------------\n";
        if (isPrefixCode(codes)) {
            std::cout << " The generated code is a valid prefix code.\n";
        } else {
            std::cout << " The generated code is not a valid prefix code.\n";
        }
        std::cout << "---------------------------------------------------------------\n\n";

        const double ent = entropy(prob);
        const double avgLen = averageLength(codes, prob);
        const double eff = efficiency(ent, avgLen);

        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Entropy for the given data is                              : " << ent << " bits\n";
        std::cout << "Average length for Shannon Fano Elias Code is              : " << avgLen << " bits\n";
        std::cout << "Efficiency of the coding scheme for the given data set is  : " << eff << " \n\n";
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}
