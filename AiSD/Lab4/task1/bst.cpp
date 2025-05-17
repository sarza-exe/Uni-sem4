#include <iostream>
#include <algorithm>
#include <vector>
#include <random>

static constexpr const char* RESET_COLOR = "\033[0m";
static constexpr const char* BLUE_COLOR  = "\033[34m";
static constexpr const char* RED_COLOR   = "\033[31m";

// Structure of a node in BST
struct Node {
    int key;
    Node* left;
    Node* right;
    Node(int value) : key(value), left(nullptr), right(nullptr) {}
};

// BST
class BST {
private:
    Node* root;

    Node* insert(Node* node, int k) {
        if (node == nullptr) {
            return new Node(k);
        }
        if (k < node->key) {
            node->left = insert(node->left, k);
        } else {
            node->right = insert(node->right, k);
        }
        return node;
    }

    // Find min key for given node
    Node* findMin(Node* node) {
        while (node->left != nullptr)
            node = node->left;
        return node;
    }

    Node* remove(Node* node, int k) {
        if (node == nullptr) {
            return nullptr; // key not found
        }
        if (k < node->key) {
            node->left = remove(node->left, k);
        } else if (k > node->key) {
            node->right = remove(node->right, k);
        } else {
            // found it
            if (node->left == nullptr && node->right == nullptr) {
                delete node;
                return nullptr;
            } else if (node->left == nullptr) {
                Node* temp = node->right;
                delete node;
                return temp;
            } else if (node->right == nullptr) {
                Node* temp = node->left;
                delete node;
                return temp;
            } else {
                // two children - replace with min in right subtree
                Node* temp = findMin(node->right);
                node->key = temp->key;
                node->right = remove(node->right, temp->key);
            }
        }
        return node;
    }

    int height(Node* node) {
        if (node == nullptr) {
            return 0;
        }
        int lh = height(node->left);
        int rh = height(node->right);
        return std::max(lh, rh) + 1;
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
    // constructor
    BST() : root(nullptr) {}

    void insert(int k) {
        root = insert(root, k);
    }

    void remove(int k) {
        root = remove(root, k);
    }

    int height() {
        return height(root);
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
    BST tree;
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


    //test 2
    std::cout << "\n\nINSERTING RANDOM PERM OF 1, 2, ..., 30 AND THEN DELETING RANDOM PERM OF 1, 2, ..., 30\n";
    auto perm2 = random_permutation(n);
    for (int i = 0; i < n; ++i) {
        std::cout << "insert " << perm2[i] << "\n";
        tree.insert(perm2[i]);
        tree.print();
    }

    auto perm3 = random_permutation(n);
    for (int i = 0; i < n; ++i) {
        std::cout << "delete " << perm3[i] << "\n";
        tree.remove(perm3[i]);
        tree.print();
    }

    return 0;
}