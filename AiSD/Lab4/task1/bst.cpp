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

    Node* remove(Node* root, int key){
        Node* curr = root;
        Node* prev = NULL;

        while (curr != NULL && curr->key != key) {
            prev = curr;
            if (key < curr->key)
                curr = curr->left;
            else
                curr = curr->right;
        }

        if (curr == NULL) 
            return root;

        // the node to be  deletedhas atmost one child.
        if (curr->left == NULL || curr->right == NULL) {
            Node* newCurr;

            // if the left child does not exist.
            if (curr->left == NULL)
                newCurr = curr->right;
            else
                newCurr = curr->left;

            // the node to be deleted is the root.
            if (prev == NULL)
                return newCurr;

            if (curr == prev->left)
                prev->left = newCurr;
            else
                prev->right = newCurr;

            delete curr;
        }
        else { // node to be deleted has two children.
            // Compute the inorder successor
            Node* p = NULL;
            Node* temp = curr->right;
            while (temp->left != NULL) {
                p = temp;
                temp = temp->left;
            }
            if (p != NULL)
                p->left = temp->right;
            else
                curr->right = temp->right;

            curr->key = temp->key;
            
            delete temp;
        }
        return root;
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

    void inorder(Node* node) {
        if (!node) return;
        inorder(node->left);
        std::cout << node->key << " ";
        inorder(node->right);
    }

public:
    // constructor
    BST() : root(nullptr) {}

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

        if (root == nullptr) root = z;
        else if (p->key > x)
            p->left = z;
        else
            p->right = z;
    }

    void remove(int k) {
        root = remove(root, k);
    }

    int height() {
        return height(root);
    }

    void printInorder() {
        inorder(root);
        std::cout << std::endl;
    }

    void print(){
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