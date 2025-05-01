#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>

// Reads the first “token” of each line of `filename`, tries to convert it to int,
// and appends it to `out`.
void readComparisons(const std::string& filename, std::vector<int>& out) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening file: " << filename << "\n";
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string firstWord;
        if (iss >> firstWord) {
            try {
                int value = std::stoi(firstWord);
                out.push_back(value);
            } catch (const std::exception& e) {
                std::cerr << "Could not convert to int (‘" << firstWord 
                          << "’): " << e.what() << "\n";
            }
        }
    }

    file.close();
}

// Prints the header, tabulates n vs. comparisons vs. C = comparisons/log2(n),
// and then prints the average constant.
void reportComparisons(const std::string& title, const std::vector<int>& comparisons) {
    std::cout << title << "\n";
    std::cout << "n\tcomparisons\tC = comparisons / log2(n)\n";

    double totalC = 0.0;
    std::size_t idx = 0;
    for (int n = 1000; n <= 50000 && idx < comparisons.size(); n += 1000, ++idx) {
        double logn = std::log2(n);
        double C    = comparisons[idx] / logn;
        totalC     += C;

        // adjust tabbing for alignment when comparisons is large
        if(n <= 5000) std::cout 
            << n << "\t" 
            << comparisons[idx] << "\t\t" 
            << C << "\n";
    }

    // there are (50000-1000)/1000 + 1 = 50 steps
    double averageC = totalC / ((comparisons.size() < 50) ? comparisons.size() : 50);
    std::cout 
        << "Average constant C (with log2): " 
        << averageC 
        << "\n\n";
}


int main() {
    std::vector<int> comparisons;
    std::vector<int> comparisons2;
    std::vector<int> comparisons3;

    readComparisons("data/bsNone.txt",       comparisons);
    readComparisons("data/bsRandom.txt",     comparisons2);
    readComparisons("data/bsBeginning.txt",  comparisons3);

    std::cout << "Binary Search Constants\n\n";

    reportComparisons("For nonexistent element:", comparisons);
    reportComparisons("For random element:",      comparisons2);
    reportComparisons("For beginning element:",   comparisons3);

    return 0;
}