#ifndef BINOMIALHEAP
#define BINOMIALHEAP

#include <limits>

/*
Implementation of Binomial Tree
A Binomial Tree of order 0 has 1 node. A Binomial Tree of order k can be constructed by taking two binomial trees of order k-1 and making one the leftmost child of the other. 
A Binomial Tree of order k has the following properties:
- It has exactly 2^k nodes.
- Its depth is k.
- There are exactly kCi nodes at depth i for i = 0, 1, . . . , k.
- The root has degree k and its children are themselves Binomial Trees of orders k-1, k-2, …, 0 from left to right.
Binomial Heap that provides faster union or merge operation with other operations provided by Binary Heap
*/

struct BinomialNode {
    int key;
    int degree;
    BinomialNode* parent;
    BinomialNode* child;
    BinomialNode* sibling;

    BinomialNode(int k) : key(k), degree(0), parent(nullptr), child(nullptr), sibling(nullptr) {}
};

class BinomialHeap {
public:
    BinomialHeap() : head(nullptr), comparisons(0) {}

    // Returns the number of comparisons made in this heap
    long long getComparisons() const { return comparisons; }

    // Insert a new key into the heap
    void insert(int key) {
        BinomialNode* node = new BinomialNode(key);
        BinomialHeap tmp;
        tmp.head = node;

        // Merge tmp with the current heap – compare() will count comparisons
        this->unionWith(tmp);
    }

    // O(log n)
    //  union this heap with another
    void unionWith(BinomialHeap& other) {
        // Merge root lists (sorted by degree) – no key comparisons here
        BinomialNode* merged = mergeRootLists(this->head, other.head);
        this->head = nullptr;
        other.head = nullptr;

        if (!merged) {
            // Both heaps were empty
            this->head = nullptr;
            return;
        }

        // Traverse merged root list and link trees of equal degree
        BinomialNode* prev_x = nullptr;
        BinomialNode* x = merged;
        BinomialNode* next_x = x->sibling;

        while (next_x) {
            if (x->degree != next_x->degree ||
                (next_x->sibling && next_x->sibling->degree == x->degree)) {
                // Move forward
                prev_x = x;
                x = next_x;
            } else {
                // We must link two trees of the same degree
                if (compare(x->key, next_x->key)) {
                    // x->key <= next_x->key → next_x becomes child of x
                    x->sibling = next_x->sibling;
                    linkTrees(next_x, x);
                } else {
                    // next_x->key < x->key → x becomes child of next_x
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

    // 2log(n)
    // Extract-min: remove and return the smallest key
    int extractMin() {
        if (!head) {
            return std::numeric_limits<int>::max(); // heap is empty
        }

        // Find root with minimum key (counting comparisons)
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

        // Remove min_node from the root list
        if (!prev_min) {
            // min_node was the first element
            head = min_node->sibling;
        } else {
            prev_min->sibling = min_node->sibling;
        }

        // Reverse child list of min_node (no comparisons) to form a new heap
        BinomialNode* child = min_node->child;
        BinomialNode* new_head = nullptr;
        while (child) {
            BinomialNode* next_child = child->sibling;
            child->sibling = new_head;
            child->parent = nullptr;
            new_head = child;
            child = next_child;
        }

        // Merge the new heap with the current one (compare() counts comparisons)
        BinomialHeap tmp;
        tmp.head = new_head;
        this->unionWith(tmp);

        // Delete only min_node – do not recursively delete children,
        // since they were merged into tmp and then into this.
        delete min_node;
        return min_key;
    }

private:
    BinomialNode* head;
    long long comparisons;

    // Compare key1 <= key2 and count the comparison
    bool compare(int key1, int key2) {
        comparisons++;
        return key1 <= key2;
    }

    // Make y a child of z (assume y->key >= z->key)
    void linkTrees(BinomialNode* y, BinomialNode* z) {
        y->parent = z;
        y->sibling = z->child;
        z->child = y;
        z->degree += 1;
    }

    // it's like merge in mergesort
    // Merge two root lists (sorted by degree), no key comparisons
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

#endif