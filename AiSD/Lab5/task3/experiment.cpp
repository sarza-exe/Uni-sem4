#include <iostream>
#include <fstream>
#include <random>
#include <iomanip>
#include "binomialHeap.h"

int main() {
    // Prepare CSV output
    std::ofstream ofs("avg_comparisons_per_op.csv");
    if (!ofs.is_open()) {
        std::cerr << "Cannot open output CSV file.\n";
        return 1;
    }

    // Write CSV header
    ofs << "n,avg_comparisons\n";

    std::mt19937_64 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 100000);

    // For each n in {100, 200, ..., 10000}
    for (int n = 100; n <= 10000; n += 100) {
        BinomialHeap H1, H2;

        // Insert n random elements into H1
        for (int i = 0; i < n; ++i) {
            int key = dist(rng);
            H1.insert(key);
        }

        // Insert n random elements into H2
        for (int i = 0; i < n; ++i) {
            int key = dist(rng);
            H2.insert(key);
        }

        // Union H1 and H2
        H1.unionWith(H2);

        // Extract-min 2n times from the merged heap (now in H1)
        for (int i = 0; i < 2 * n; ++i) {
            H1.extractMin();
        }

        // Total comparisons = comparisons in H1 (all its operations) + comparisons in H2 (its insert operations)
        long long totalComparisons = H1.getComparisons() + H2.getComparisons();

        // Compute average cost per n
        double avg = static_cast<double>(totalComparisons) / n;

        // Write line: n,avg
        ofs << n << "," << std::fixed << std::setprecision(3) << avg << "\n";

        std::cout << "n=" << n << "  avg_comparisons=" << avg << "\n";
    }

    ofs.close();
    std::cout << "Results written to avg_comparisons_per_op.csv\n";
    return 0;
}
