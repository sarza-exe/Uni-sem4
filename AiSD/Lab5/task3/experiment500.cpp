#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <limits>
#include "binomialHeap.h"

void performExperiments(int n = 500, int numExperiments = 5, const std::string& outputFile = "experiments.csv") {
    std::mt19937_64 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 10000);

    std::ofstream ofs(outputFile);
    if (!ofs.is_open()) {
        std::cerr << "Cannot open file for writing: " << outputFile << std::endl;
        return;
    }

    // For each experiment, create a vector of comparison counts per operation
    for (int exp = 0; exp < numExperiments; ++exp) {
        BinomialHeap H1, H2;
        std::vector<long long> counts;
        counts.reserve(4 * n + 1);

        // Insert n random elements into H1
        for (int i = 0; i < n; ++i) {
            int key = dist(rng);
            long long before = H1.getComparisons();
            H1.insert(key);
            long long after = H1.getComparisons();
            counts.push_back(after - before);
        }

        // Insert n random elements into H2
        for (int i = 0; i < n; ++i) {
            int key = dist(rng);
            long long before = H2.getComparisons();
            H2.insert(key);
            long long after = H2.getComparisons();
            counts.push_back(after - before);
        }

        // Merge H1 and H2 (union)
        long long beforeUnion = H1.getComparisons();
        H1.unionWith(H2);
        long long afterUnion = H1.getComparisons();
        counts.push_back(afterUnion - beforeUnion);

        // Perform 2n extract-min operations on the merged heap H1
        for (int i = 0; i < 2 * n; ++i) {
            long long before = H1.getComparisons();
            H1.extractMin();
            long long after = H1.getComparisons();
            counts.push_back(after - before);
        }

        // Write the row to CSV
        for (size_t i = 0; i < counts.size(); ++i) {
            ofs << counts[i];
            if (i + 1 < counts.size()) {
                ofs << ",";
            }
        }
        ofs << "\n";
    }

    ofs.close();
    std::cout << "CSV results saved to: " << outputFile << std::endl;
}

int main() {
    const int n = 500;
    const int numExperiments = 5;
    performExperiments(n, numExperiments, "experiments.csv");
    return 0;
}
