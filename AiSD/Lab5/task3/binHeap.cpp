#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <limits>

//---------------------------------------------
// Implementacja kopca dwumianowego w C++
//---------------------------------------------

struct BinomialNode {
    int key;
    int degree;
    BinomialNode* parent;
    BinomialNode* child;
    BinomialNode* sibling;

    BinomialNode(int k)
        : key(k), degree(0), parent(nullptr), child(nullptr), sibling(nullptr) {}
};

class BinomialHeap {
public:
    BinomialHeap() : head(nullptr), comparisons(0) {}

    // Zwraca liczbę porównań wykonanych w tym kopcu
    long long getComparisons() const { return comparisons; }

    // Wstawienie pojedynczego klucza (heap-insert)
    void insert(int key) {
        BinomialNode* node = new BinomialNode(key);
        BinomialHeap tmp;
        tmp.head = node;

        // Scal tmp z bieżącym kopcem – _compare() zlicza porównania
        this->unionWith(tmp);
        // Nie zwalniamy tu node ani węzłów tmp – one zostały włączone do this.
    }

    // Scalanie (union) bieżącego kopca z „other”
    void unionWith(BinomialHeap& other) {
        // 1) Scal root listy (posortowane po degree) – bez porównań kluczy
        BinomialNode* merged = mergeRootLists(this->head, other.head);
        this->head = nullptr;
        other.head = nullptr;

        if (!merged) {
            // Oba kopce były puste
            this->head = nullptr;
            return;
        }

        // 2) Idziemy po scalonej liście korzeni i łączymy drzewa o tym samym degree
        BinomialNode* prev_x = nullptr;
        BinomialNode* x = merged;
        BinomialNode* next_x = x->sibling;

        while (next_x) {
            if (x->degree != next_x->degree ||
                (next_x->sibling && next_x->sibling->degree == x->degree)) {
                // Przechodzimy dalej
                prev_x = x;
                x = next_x;
            } else {
                // Musimy połączyć dwa drzewa o tych samych degree
                if (compare(x->key, next_x->key)) {
                    // x->key <= next_x->key → next_x staje się dzieckiem x
                    x->sibling = next_x->sibling;
                    linkTrees(next_x, x);
                } else {
                    // next_x->key < x->key → x staje się dzieckiem next_x
                    if (!prev_x) {
                        merged = next_x;
                    } else {
                        prev_x->sibling = next_x;
                    }
                    linkTrees(x, next_x);
                    x = next_x;
                }
            }
            next_x = x->sibling;
        }

        this->head = merged;
    }

    // Extract-min: usuwa i zwraca najmniejszy klucz
    int extractMin() {
        if (!head) {
            return std::numeric_limits<int>::max(); // kopiec pusty
        }

        // A) Znajdź korzeń z najmniejszym kluczem (zliczamy porównania)
        BinomialNode* prev_min = nullptr;
        BinomialNode* min_node = head;
        BinomialNode* prev_x = nullptr;
        BinomialNode* x = head;
        int min_key = x->key;

        while (x->sibling) {
            prev_x = x;
            x = x->sibling;
            if (compare(x->key, min_key)) {
                min_key = x->key;
                min_node = x;
                prev_min = prev_x;
            }
        }

        // B) Usuń min_node z listy korzeni
        if (!prev_min) {
            // min_node było pierwszym elementem
            head = min_node->sibling;
        } else {
            prev_min->sibling = min_node->sibling;
        }

        // C) Odwróć listę dzieci min_node (bez porównań) i przygotuj tymczasowy kopiec
        BinomialNode* child = min_node->child;
        BinomialNode* new_head = nullptr;
        while (child) {
            BinomialNode* next_child = child->sibling;
            child->sibling = new_head;
            child->parent = nullptr;
            new_head = child;
            child = next_child;
        }

        // D) Scal nowy kopiec z bieżącym (compare() zlicza porównania)
        BinomialHeap tmp;
        tmp.head = new_head;
        this->unionWith(tmp);

        // Usuwamy tylko min_node – nie usuwamy już rekurencyjnie dzieci,
        // bo zostały włączone do tmp i stąd do this.
        delete min_node;
        return min_key;
    }

private:
    BinomialNode* head;
    long long comparisons;

    // Porównuje key1 <= key2, zlicza porównanie
    bool compare(int key1, int key2) {
        comparisons++;
        return key1 <= key2;
    }

    // Sprawia, że y staje się dzieckiem z (zakładamy: y->key >= z->key)
    void linkTrees(BinomialNode* y, BinomialNode* z) {
        y->parent = z;
        y->sibling = z->child;
        z->child = y;
        z->degree += 1;
    }

    // Scalanie dwóch list korzeni (posortowanych według degree), bez porównań kluczy
    static BinomialNode* mergeRootLists(BinomialNode* h1, BinomialNode* h2) {
        if (!h1) return h2;
        if (!h2) return h1;

        BinomialNode* head;
        BinomialNode* tail;
        if (h1->degree <= h2->degree) {
            head = h1;
            h1 = h1->sibling;
        } else {
            head = h2;
            h2 = h2->sibling;
        }
        tail = head;

        while (h1 && h2) {
            if (h1->degree <= h2->degree) {
                tail->sibling = h1;
                h1 = h1->sibling;
            } else {
                tail->sibling = h2;
                h2 = h2->sibling;
            }
            tail = tail->sibling;
        }
        tail->sibling = (h1 ? h1 : h2);
        return head;
    }
};

//---------------------------------------------
// Funkcja przeprowadzająca eksperymenty i zapis CSV
//---------------------------------------------
void performExperiments(int n = 500, int numExperiments = 5, const std::string& outputFile = "experiments.csv") {
    std::mt19937_64 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 10000);

    std::ofstream ofs(outputFile);
    if (!ofs.is_open()) {
        std::cerr << "Nie można otworzyć pliku do zapisu: " << outputFile << std::endl;
        return;
    }

    // Dla każdego eksperymentu tworzymy wektor liczby porównań per operacja
    for (int exp = 0; exp < numExperiments; ++exp) {
        BinomialHeap H1, H2;
        std::vector<long long> counts;
        counts.reserve(4 * n + 1);

        // 1) Wstawienie n losowych elementów do H1
        for (int i = 0; i < n; ++i) {
            int key = dist(rng);
            long long before = H1.getComparisons();
            H1.insert(key);
            long long after = H1.getComparisons();
            counts.push_back(after - before);
        }

        // 2) Wstawienie n losowych elementów do H2
        for (int i = 0; i < n; ++i) {
            int key = dist(rng);
            long long before = H2.getComparisons();
            H2.insert(key);
            long long after = H2.getComparisons();
            counts.push_back(after - before);
        }

        // 3) Scal H1 i H2 (union)
        long long beforeUnion = H1.getComparisons();
        H1.unionWith(H2);
        long long afterUnion = H1.getComparisons();
        counts.push_back(afterUnion - beforeUnion);

        // 4) Wykonanie 2n operacji Extract-Min na scalonym kopcu H1
        for (int i = 0; i < 2 * n; ++i) {
            long long before = H1.getComparisons();
            H1.extractMin();
            long long after = H1.getComparisons();
            counts.push_back(after - before);
        }

        // Zapisujemy wiersz do CSV
        for (size_t i = 0; i < counts.size(); ++i) {
            ofs << counts[i];
            if (i + 1 < counts.size()) {
                ofs << ",";
            }
        }
        ofs << "\n";
    }

    ofs.close();
    std::cout << "Plik CSV z wynikami zapisano jako: " << outputFile << std::endl;
}

//---------------------------------------------
// Główna funkcja
//---------------------------------------------
int main() {
    const int n = 500;
    const int numExperiments = 5;
    performExperiments(n, numExperiments, "experiments.csv");
    return 0;
}
