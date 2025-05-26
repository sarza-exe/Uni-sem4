#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <queue>

static constexpr const char* RESET_COLOR = "\033[0m";
static constexpr const char* BLUE_COLOR  = "\033[34m";
static constexpr const char* RED_COLOR   = "\033[31m";

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
        if (!root) return 0;
        std::queue<Node*> q;
        q.push(root);
        int h = 0;
        while (!q.empty()) {
            int sz = q.size();
            while (sz--) {
                Node* u = q.front(); q.pop();
                if (u->left)  q.push(u->left);
                if (u->right) q.push(u->right);
            }
            ++h;
        }
        return h;
    }

    void leftRotate(Node* x){
        Node* y = x->right;
        x->right = y->left;
        if(y->left != nullptr){
            y->left->parent = x;
        }
        y->parent = x->parent;
        if(x->parent == nullptr){
            root = y;
        } 
        else if(x == x->parent->left){
            x->parent->left = y;
        } 
        else{
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;
    }

    void rightRotate(Node* y){
        Node* x = y->left;
        y->left = x->right;
        if(x->right != nullptr){
            x->right->parent = y;
        }
        x->parent = y->parent;
        if(y->parent == nullptr){
            root = x;
        } 
        else if(y == y->parent->left){
            y->parent->left = x;
        } 
        else{
            y->parent->right = x;
        }
        x->right = y;
        y->parent = x;
    }

    // Splay node x to root
    void splay(Node* x) {
        while (x->parent) {
            Node* y = x->parent;
            Node* z = y->parent;
            if (!z) {
                // Single step
                if (x == y->left) {
                    rightRotate(y);  // Zig
                } else {
                    leftRotate(y);   // Zag
                }
            } else if ((x == y->left) && (y == z->left)) {
                // Zig-Zig
                rightRotate(z);
                rightRotate(y);
            } else if ((x == y->right) && (y == z->right)) {
                // Zag-Zag
                leftRotate(z);
                leftRotate(y);
            } else if ((x == y->right) && (y == z->left)) {
                // Zig-Zag
                leftRotate(y);
                rightRotate(z);
            } else if ((x == y->left) && (y == z->right)) {
                // Zag-Zig
                rightRotate(y);
                leftRotate(z);
            }
            else {
                std::cout << "Massive failure :D\n";
                return;
            }
        }
    }

    Node* find(int k) {
        Node* x = root;
        while (x) {
            if (k < x->key)  x = x->left;
            else if (k > x->key) x = x->right;
            else {
                splay(x);
                return x;
            }
        }
        return nullptr;
    }

    //Replaces subtree rooted at u with subtree rooted at v
    void transplant(Node* u, Node* v){
        if(u->parent == nullptr){
            root = v;
        }
        else if(u == u->parent->left){
            u->parent->left = v;
        }
        else{
            u->parent->right = v;
        }
        if (v) {
            v->parent = u->parent;
        }
    }

    void print(const std::string& prefix, const Node* node, bool isLeft)
    {
        if( node != nullptr )
        {
            const char* color = isLeft ? BLUE_COLOR : RED_COLOR;
            std::cout << prefix;
            std::cout << (isLeft ? "├──" : "└──" );
            // print the value of the node
            std::cout << color << node->key << RESET_COLOR << "\n";
            // enter the next tree level - left and right branch
            print( prefix + (isLeft ? "│   " : "    "), node->left, true);
            print( prefix + (isLeft ? "│   " : "    "), node->right, false);
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
            if (curr->key > x)
                curr = curr->left;
            else {
                curr = curr->right;
            }
        }

        // Set the parent of the new node
        z->parent = p;

        if (root == nullptr) root = z;
        else if (p->key > x)
            p->left = z;
        else
            p->right = z;
        
        splay(z);
    }

    void remove(int key) {
        Node* z = find(key);
        if (!z) return; // Key not found

        Node* parent = z->parent;

        if (!z->left) {
            transplant(z, z->right);
        } else if (!z->right) {
            transplant(z, z->left);
        } else {
            Node* y = z->right;
            while (y->left) {
                y = y->left;
            }
            if (y->parent != z) {
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

        if (parent)
            splay(parent);
    }

    int height() {
        return heightIterative();
    }

    void inorder(Node* node) {
        if (!node) return;
        inorder(node->left);
        std::cout << node->key << " ";
        inorder(node->right);
    }

    void printInorder() {
        inorder(root);
        std::cout << std::endl;
    }

    void print()
    {
        print("", root, false);    
    }
};

std::vector<int> random_permutation(int n) {
    std::vector<int> perm(n);
    for (int i = 0; i < n; ++i) {
        perm[i] = i + 1;
    }
    std::random_device rd; 
    std::mt19937 gen(rd()); 
    std::shuffle(perm.begin(), perm.end(), gen);
    return perm;
}

int main() {
    ST tree;
    const int n = 30;
    
    //test 1
    std::cout << "INSERTING 1, 2, ..., 30 AND DELETING RANDOM PERM OF 1, 2, ..., 30\n";
    for (int i = 0; i < n; ++i) {
        std::cout << "insert " << i+1 << "\n";
        tree.insert(i+1);
        tree.print();
    }

    auto perm1 = random_permutation(n);
    for (int i = 0; i < n; ++i) {
        std::cout << "delete " << perm1[i] << "\n";
        tree.remove(perm1[i]);
        tree.print();
    }


    // test 2
    std::cout << "\n\nINSERTING RANDOM PERM OF 1, 2, ..., 30 AND THEN DELETING RANDOM PERM OF 1, 2, ..., 30\n";
    auto perm2 = random_permutation(n);
    for (int i = 0; i < n; ++i) {
        std::cout << "insert " << perm2[i] << "\n";
        tree.insert(perm2[i]);
        tree.print();
        tree.printInorder();
    }

    auto perm3 = random_permutation(n);
    for (int i = 0; i < n; ++i) {
        std::cout << "delete " << perm3[i] << "\n";
        tree.remove(perm3[i]);
        tree.print();
    }

    return 0;
}