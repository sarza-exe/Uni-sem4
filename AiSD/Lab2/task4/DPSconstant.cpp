#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>

int main() {
    std::ifstream file("data/DPQuickSortDataBig.txt");
    std::vector<int> comparisons;

    if (!file.is_open()) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string firstWord;
        if (iss >> firstWord) {
            try {
                int value = std::stoi(firstWord);
                comparisons.push_back(value);
            } catch (...) {
                std::cerr << "Could not convert to int: " << firstWord << "\n";
            }
        }
    }

    file.close();

    std::cout << "n\tcomparisons\tC = comparisons / (n * log2(n))\n";

    int index = 0;
    double constant = 0;
    for(int n = 1000; n <= 50000; n += 1000)
    {
        double logn = std::log2(n);
        double C = comparisons[index] / (n * logn);

        std::cout << n << "\t" << comparisons[index] << "\t\t" << C << "\n";
        index++;
        constant += C;
    }
    std::cout<< "Wartosc stalej przy nlog2(n) wynosi: " << constant/50 << "\n";

    return 0;
}
