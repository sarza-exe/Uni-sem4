#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <fstream>
#include <string>
#include <queue>

/*
Przeprowadź eksperymenty badające złożoność dla dużych danych (po 20 testów dla
wartości n równych: 10 000, 20 000, ..., 100 000), dla takich samych scenariuszy jak w
Zadaniu 1. (tj. wstawianie rosnącego ciągu kluczy 1, 2, ..., n i usuwanie losowej permutacji
tego ciągu oraz wstawianie losowej permutacji ciągu kluczy 1, 2, ..., n i usuwanie losowej
permutacji tego ciągu) ale bez wyświetlania wykonywanych operacji i drzew.
Jako miary złożoności każdej operacji zliczaj:
● liczby porównań między kluczami,
● liczby odczytów i podstawień wskaźników łączących elementy struktury drzewa,
● wysokość drzewa po każdej operacji.
Dla każdego n zliczaj zarówno średni koszt, jak i maksymalny napotkany koszt pojedynczej
operacji.
Przygotuj obrazy z wykresami uzyskanych wyników dla każdej z tych miar.
*/

static uint64_t cnt_comparisons = 0;
static uint64_t cnt_pointer_reads = 0;
static uint64_t cnt_pointer_writes = 0;

void reset_counters() {
    cnt_comparisons = 0;
    cnt_pointer_reads = 0;
    cnt_pointer_writes = 0;
}

struct Node {
    int key;
    Node* left;
    Node* right;
    Node(int v) : key(v), left(nullptr), right(nullptr) {
        cnt_pointer_writes += 2;
    }
};

class BST {
private:
    Node* root;

    int height_iterative() {
        if (!root) return 0;
        std::queue<Node*> q;
        q.push(root);
        int h = 0;
        while (!q.empty()) {
            int sz = q.size();
            while (sz--) {
                Node* u = q.front(); q.pop();
                cnt_pointer_reads += 2;
                if (u->left)  q.push(u->left);
                if (u->right) q.push(u->right);
            }
            ++h;
        }
        return h;
    }

    Node* insert(Node* root, int x){
        cnt_pointer_writes += 1;
        Node* temp = new Node(x);

        // If tree is empty
        if (root == NULL)
            return temp;

        // Find the node who is going to have the new node temp as
        // it child. The parent node is mainly going to be a leaf node
        Node *parent = NULL, *curr = root;
        while (curr != NULL) {
            parent = curr;
            cnt_pointer_reads += 1;
            cnt_comparisons += 1;
            if (curr->key > x)
                curr = curr->left;
            else if (curr->key < x){
                cnt_pointer_reads += 1;
                cnt_comparisons += 1;
                curr = curr->right;
            }
            else{
                cnt_pointer_reads += 1;
                cnt_comparisons += 1;
                return root;
            }
        }

        // If x is smaller, make it left child, else right child
        cnt_pointer_reads += 1;
        cnt_comparisons += 1;
        if (parent->key > x)
            parent->left = temp;
        else
            parent->right = temp;
        return root;
    }

    // find min in subtree
    Node* findMin(Node* node) {
        while (true) {
            cnt_pointer_reads += 1;
            if (node->left == nullptr) break;
            cnt_pointer_reads += 1;
            node = node->left;
        }
        return node;
    }

    Node* remove(Node* root, int key){
        Node* curr = root;
        Node* prev = NULL;

        // Check if key in tree 
        // prev points to the parent of the key to be deleted.
        cnt_pointer_reads += 1;
        cnt_comparisons += 1;
        while (curr != NULL && curr->key != key) {
            prev = curr;
            cnt_pointer_reads += 2;
            cnt_comparisons += 1;
            if (key < curr->key)
                curr = curr->left;
            else
                curr = curr->right;
        }

        // Key not present
        if (curr == NULL) 
            return root;

        // Check if node to be deleted has at most one child.
        cnt_pointer_reads += 2;
        if (curr->left == NULL || curr->right == NULL) {

            // newCurr will replace the node to be deleted.
            Node* newCurr;

            cnt_pointer_reads += 2;
            if (curr->left == NULL)
                newCurr = curr->right;
            else
                newCurr = curr->left;

            // node to be deleted is the root.
            if (prev == NULL)
                return newCurr;

            cnt_pointer_reads += 1;
            cnt_pointer_writes += 1;
            if (curr == prev->left)
                prev->left = newCurr;
            else
                prev->right = newCurr;

            // free memory of the node to be deleted.
            delete curr;
        }
        else { // node to be deleted has two children.
            // Compute the inorder successor
            Node* p = NULL;
            cnt_pointer_reads += 1;
            Node* temp = curr->right;
            while (temp->left != NULL) {
                cnt_pointer_reads += 1;
                p = temp;
                temp = temp->left;
            }
            cnt_pointer_reads += 1;

            // check if the parent of the inorder
            // successor is the curr or not(i.e. curr=
            // the node which has the same data as
            // the given data by the user to be
            // deleted). if it isn't, then make the
            // the left child of its parent equal to
            // the inorder successor'd right child.
            cnt_pointer_reads += 1;
            cnt_pointer_writes += 1;
            if (p != NULL)
                p->left = temp->right;
            else
                curr->right = temp->right;

            cnt_pointer_reads += 1;
            cnt_pointer_writes += 1;
            curr->key = temp->key;
            
            delete temp;
        }
        return root;
    }

public:
    BST() : root(nullptr) {
        cnt_pointer_writes += 1;
    }

    void insert(int k) {
        root = insert(root, k);
        cnt_pointer_writes += 1;
    }

    void remove(int k) {
        root = remove(root, k);
        cnt_pointer_writes += 1;
    }

    int height() {
        return height_iterative();
    }
};

std::vector<int> random_permutation(int n) {
    std::vector<int> perm(n);
    for (int i = 0; i < n; ++i) perm[i] = i + 1;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::shuffle(perm.begin(), perm.end(), gen);
    return perm;
}


int main() {
    std::ofstream csv("bst_stats.csv");
    csv << "scenario,n,op,avg_comp,max_comp,avg_reads,max_reads,avg_writes,max_writes,avg_height,max_height\n";

    std::vector<int> ns;
    for (int v = 10000; v <= 100000; v += 10000) ns.push_back(v);
    const int trials = 20;
    enum Scenario { INCR_SEQ, RAND_SEQ };

    for (int n : ns) {
        for (int scen = INCR_SEQ; scen <= RAND_SEQ; ++scen) {
            // stats
            std::vector<uint64_t> comp_ins, comp_rem;
            std::vector<uint64_t> reads_ins, reads_rem;
            std::vector<uint64_t> writes_ins, writes_rem;
            std::vector<int> height_ins, height_rem;

            for (int t = 0; t < trials; ++t) {
                reset_counters();
                BST tree;

                // Przygotuj sekwencję do wstawiania
                std::vector<int> seq = (scen == INCR_SEQ)
                    ? std::vector<int>(n) : random_permutation(n);
                if (scen == INCR_SEQ) for (int i = 0; i < n; ++i) seq[i] = i + 1;

                // Wstawianie
                uint64_t max_ci = 0, sum_ci = 0;
                uint64_t max_ri = 0, sum_ri = 0;
                uint64_t max_wi = 0, sum_wi = 0;
                int max_hi = 0, sum_hi = 0;
                for (int k : seq) {
                    uint64_t before_c = cnt_comparisons;
                    uint64_t before_r = cnt_pointer_reads;
                    uint64_t before_w = cnt_pointer_writes;

                    tree.insert(k);
                    int h = tree.height();

                    uint64_t dc = cnt_comparisons - before_c;
                    uint64_t dr = cnt_pointer_reads - before_r;
                    uint64_t dw = cnt_pointer_writes - before_w;

                    sum_ci += dc; max_ci = std::max(max_ci, dc);
                    sum_ri += dr; max_ri = std::max(max_ri, dr);
                    sum_wi += dw; max_wi = std::max(max_wi, dw);
                    sum_hi += h; max_hi = std::max(max_hi, h);
                }
                comp_ins.push_back(sum_ci / n); comp_ins.push_back(max_ci);
                reads_ins.push_back(sum_ri / n); reads_ins.push_back(max_ri);
                writes_ins.push_back(sum_wi / n); writes_ins.push_back(max_wi);
                height_ins.push_back(sum_hi / n); height_ins.push_back(max_hi);

                // Przygotuj permutację do usuwania
                std::vector<int> rem_seq = random_permutation(n);

                // Usuwanie
                uint64_t max_cr = 0, sum_cr = 0;
                uint64_t max_rr = 0, sum_rr = 0;
                uint64_t max_wr = 0, sum_wr = 0;
                int max_hr2 = 0, sum_hr2 = 0;
                for (int k : rem_seq) {
                    uint64_t before_c = cnt_comparisons;
                    uint64_t before_r = cnt_pointer_reads;
                    uint64_t before_w = cnt_pointer_writes;

                    tree.remove(k);
                    int h = tree.height();

                    uint64_t dc = cnt_comparisons - before_c;
                    uint64_t dr = cnt_pointer_reads - before_r;
                    uint64_t dw = cnt_pointer_writes - before_w;

                    sum_cr += dc; max_cr = std::max(max_cr, dc);
                    sum_rr += dr; max_rr = std::max(max_rr, dr);
                    sum_wr += dw; max_wr = std::max(max_wr, dw);
                    sum_hr2 += h; max_hr2 = std::max(max_hr2, h);
                }
                comp_rem.push_back(sum_cr / n); comp_rem.push_back(max_cr);
                reads_rem.push_back(sum_rr / n); reads_rem.push_back(max_rr);
                writes_rem.push_back(sum_wr / n); writes_rem.push_back(max_wr);
                height_rem.push_back(sum_hr2 / n); height_rem.push_back(max_hr2);
            }
            std::cout << "N = " << n << " BITCHES\n";

            // Zapis wyników do CSV
            std::string scname = (scen == INCR_SEQ) ? "increasing_insert" : "random_insert";
            csv << scname << "," << n << ",insert,"
                << comp_ins[0]  << "," << comp_ins[1]  << ","
                << reads_ins[0] << "," << reads_ins[1] << ","
                << writes_ins[0]<< "," << writes_ins[1]<< ","
                << height_ins[0]<< "," << height_ins[1]<< "\n";

            csv << scname << "," << n << ",remove,"
                << comp_rem[0]  << "," << comp_rem[1]  << ","
                << reads_rem[0] << "," << reads_rem[1] << ","
                << writes_rem[0]<< "," << writes_rem[1]<< ","
                << height_rem[0]<< "," << height_rem[1]<< "\n";
        }
    }

    csv.close();
    std::cout << "Eksperyment zakończony. Wyniki zapisano do bst_stats.csv\n";
    return 0;
}