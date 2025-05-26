#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <fstream>
#include <string>
#include <queue>
#include <windows.h>

static uint64_t cnt_comparisons = 0;
static uint64_t cnt_pointer_reads = 0;
static uint64_t cnt_pointer_writes = 0;

void reset_counters() {
    cnt_comparisons = 0;
    cnt_pointer_reads = 0;
    cnt_pointer_writes = 0;
}

// Structure of a node in ST
struct Node {
    int key;
    Node* left;
    Node* right;
    Node* parent;
    Node(int value) : key(value), left(nullptr), right(nullptr), parent(nullptr) {}
};

// ST
class ST {
private:
    Node* root;

    int heightIterative() {
        if (root == nullptr) return 0;
        std::queue<Node*> q;
        q.push(root);
        int h = 0;
        while (!q.empty()) {
            int sz = q.size();
            while (sz--) {
                Node* u = q.front(); q.pop();
                cnt_pointer_reads += 2;
                if (u->left != nullptr)  q.push(u->left);
                if (u->right != nullptr) q.push(u->right);
            }
            ++h;
        }
        return h;
    }

    void leftRotate(Node* x){
        cnt_pointer_reads += 9;
        cnt_pointer_writes += 4;
        Node* y = x->right;
        x->right = y->left;
        if(y->left != nullptr){
            cnt_pointer_reads += 1;
            cnt_pointer_writes += 1;
            y->left->parent = x;
        }
        y->parent = x->parent;
        if(x->parent == nullptr){
            root = y;
        } 
        else if(x == x->parent->left){
            cnt_pointer_reads += 4;
            cnt_pointer_writes += 1;
            x->parent->left = y;
        } 
        else{
            cnt_pointer_reads += 4;
            cnt_pointer_writes += 1;
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;
    }

    void rightRotate(Node* y){
        cnt_pointer_reads += 9;
        cnt_pointer_writes += 4;
        Node* x = y->left;
        y->left = x->right;
        if(x->right != nullptr){
            cnt_pointer_reads += 1;
            cnt_pointer_writes += 1;
            x->right->parent = y;
        }
        x->parent = y->parent;
        if(y->parent == nullptr){
            root = x;
        } 
        else if(y == y->parent->left){
            cnt_pointer_reads += 4;
            cnt_pointer_writes += 1;
            y->parent->left = x;
        } 
        else{
            cnt_pointer_reads += 4;
            cnt_pointer_writes += 1;
            y->parent->right = x;
        }
        x->right = y;
        y->parent = x;
    }

    // Splay node x to root
    void splay(Node* x) {
        while (x->parent) {
            cnt_pointer_reads += 3;
            Node* y = x->parent;
            Node* z = y->parent;
            if (!z) {
                // Single step
                cnt_pointer_reads += 1;
                if (x == y->left) {
                    rightRotate(y);  // Zig
                } else {
                    leftRotate(y);   // Zag
                }
            } else if ((x == y->left) && (y == z->left)) {
                cnt_pointer_reads += 2;
                // Zig-Zig
                rightRotate(z);
                rightRotate(y);
            } else if ((x == y->right) && (y == z->right)) {
                cnt_pointer_reads += 4;
                // Zag-Zag
                leftRotate(z);
                leftRotate(y);
            } else if ((x == y->right) && (y == z->left)) {
                cnt_pointer_reads += 6;
                // Zig-Zag
                leftRotate(y);
                rightRotate(z);
            } else if ((x == y->left) && (y == z->right)) {
                cnt_pointer_reads += 8;
                // Zag-Zig
                rightRotate(y);
                leftRotate(z);
            }
            else {
                std::cout << "Massive failure :D\n";
                return;
            }
        }
        cnt_pointer_reads += 1;
    }

    Node* find(int k) {
        Node* x = root;
        while (x) {
            cnt_pointer_reads += 2;
            cnt_comparisons += 1;
            if (k < x->key)  x = x->left;
            else if (k > x->key) {
                x = x->right;
                cnt_pointer_reads += 1;
                cnt_comparisons += 1;
            }
            else {
                cnt_pointer_reads += 1;
                cnt_comparisons += 1;
                return x;
            }
        }
        return nullptr;
    }

    //Replaces subtree rooted at u with subtree rooted at v
    void transplant(Node* u, Node* v){
        if(u->parent == nullptr){
            cnt_pointer_reads += 1;
            root = v;
        }
        else if(u == u->parent->left){
            cnt_pointer_reads += 5;
            cnt_pointer_writes += 1;
            u->parent->left = v;
        }
        else{
            cnt_pointer_reads += 7;
            cnt_pointer_writes += 1;
            u->parent->right = v;
        }
        if (v) {
            cnt_pointer_reads += 2;
            cnt_pointer_writes += 1;
            v->parent = u->parent;
        }
    }

public:
    ST() : root(nullptr) {}

    void insert(int x){
        Node* z = new Node(x);
        Node *p = nullptr, *curr = root;

        // Traverse the tree to find the insert location
        while (curr != nullptr) {
            p = curr;
            cnt_pointer_reads += 2;
            cnt_comparisons += 1;
            if (curr->key > x)
                curr = curr->left;
            else {
                curr = curr->right;
            }
        }

        // Set the parent of the new node
        cnt_pointer_reads += 1;
        cnt_pointer_writes += 1;
        z->parent = p;

        if (root == nullptr) root = z;
        else if (p->key > x){
            cnt_comparisons += 1;
            cnt_pointer_reads += 2;
            cnt_pointer_writes += 1;
            p->left = z;
        }
        else{
            cnt_comparisons += 1;
            cnt_pointer_reads += 2;
            cnt_pointer_writes += 1;
            p->right = z;
        }
        
        splay(z);
    }

    void remove(int key) {
        Node* z = find(key);
        if (!z) return; // Key not found

        cnt_pointer_reads += 3;
        Node* parent = z->parent;

        if (!z->left) {
            transplant(z, z->right);
        } else if (!z->right) {
            cnt_pointer_reads += 1;
            transplant(z, z->left);
        } else {
            cnt_pointer_reads += 8;
            cnt_pointer_writes += 2;
            Node* y = z->right;
            while (y->left) {
                cnt_pointer_reads += 2;
                y = y->left;
            }
            if (y->parent != z) {
                cnt_pointer_reads += 5;
                cnt_pointer_writes += 2;
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            parent = y->parent;
        }
        delete z;

        if (parent) splay(parent);
    }

    int height() {
        return heightIterative();
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
    //prevent windows from sleeping
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);

    std::ofstream csv("bst_stats.csv");
    csv << "scenario,n,op,avg_comp,max_comp,avg_reads,max_reads,avg_writes,max_writes,avg_height,max_height\n";

    std::vector<int> ns;
    for (int v = 10000; v <= 100000; v += 10000) ns.push_back(v);
    const int trials = 5;
    enum Scenario { INCR_SEQ, RAND_SEQ };

    for (int n : ns) {
        for (int scen = INCR_SEQ; scen <= RAND_SEQ; ++scen) {
            // stats
            std::vector<uint64_t> comp_ins, comp_rem;
            std::vector<uint64_t> reads_ins, reads_rem;
            std::vector<uint64_t> writes_ins, writes_rem;
            std::vector<int> height_ins, height_rem;

            for (int t = 0; t < trials; ++t) {
                std::cout<<"Hello?";
                reset_counters();
                ST tree;

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

    // Clear the execution state override
    SetThreadExecutionState(ES_CONTINUOUS);

    return 0;
}