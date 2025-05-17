#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
using namespace std;

static constexpr const char* RESET_COLOR = "\033[0m";
static constexpr const char* BLACK_COLOR  = "\033[34m";
static constexpr const char* RED_COLOR   = "\033[31m";

enum Color {RED, BLACK};

struct Node;
static Node* NIL = nullptr;

struct Node{
    int key;
    Color color;
    Node* left;
    Node* right;
    Node* parent;
    Node(int value): key(value), color(RED), left(NIL), right(NIL), parent(NIL) {}
    Node() : key(0), color(BLACK), left(this), right(this), parent(this) {}
};

void init_sentinel() {
    if (NIL == nullptr) {
        NIL = new Node();  // użyje domyślnego konstruktora, kolor BLACK
    }
}

class RBT{
private:
    Node* root;

    void leftRotate(Node* x){
        Node* y = x->right;
        x->right = y->left;
        if(y->left != NIL){
            y->left->parent = x;
        }
        y->parent = x->parent;
        if(x->parent == NIL){
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
        if(x->right != NIL){
            x->right->parent = y;
        }
        x->parent = y->parent;
        if(y->parent == NIL){
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
    
    void insertFixup(Node* z){
        while(z->parent->color == RED){
            if(z->parent == z->parent->parent->left){
                Node* y = z->parent->parent->right;
                if(y->color == RED){
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                }
                else if(z == z->parent->right){
                    z = z->parent;
                    leftRotate(z);
                }
                else{
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rightRotate(z->parent->parent);
                }
            }
            else{
                Node* y = z->parent->parent->left;
                if(y->color == RED){
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                }
                else if(z == z->parent->left){
                    z = z->parent;
                    rightRotate(z);
                }
                else{
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    leftRotate(z->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }

    void insert(Node* node, int k) {
        Node* y = NIL;
        Node* x = root;
        Node* z = new Node(k);
        while (x != NIL){
            y = x;
            if(z->key < x->key)
                x = x->left;
            else x = x->right;
        }
        z->parent = y;
        if(y == NIL)
            root = z;
        else if (z->key < y->key)
            y->left = z;
        else y->right = z;
        insertFixup(z);
    }

    Node* minimum(Node* x) {
        while (x->left != NIL) {
            x = x->left;
        }
        return x;
    }

    void transplant(Node* u, Node* v){
        if(u->parent == NIL){
            root = v;
        }
        else if(u == u->parent->left){
            u->parent->left = v;
        }
        else{
            u->parent->right = v;
        }
        v->parent = u->parent;
    }

    void deleteFixup(Node* x){
        while (x != root && x->color == BLACK) {
            if (x == x->parent->left) {
                Node* w = x->parent->right;
                if (w->color == RED) { // case 1
                    w->color = BLACK;
                    x->parent->color = RED;
                    leftRotate(x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == BLACK && w->right->color == BLACK) { // case 2
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->right->color == BLACK) { // case 3
                        w->left->color = BLACK;
                        w->color = RED;
                        rightRotate(w);
                        w = x->parent->right;
                    } // case 4
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    leftRotate(x->parent);
                    x = root;
                }
            } else { // mirror image: left and right exchanged
                Node* w = x->parent->left;
                if (w->color == RED) { // case 1
                    w->color = BLACK;
                    x->parent->color = RED;
                    rightRotate(x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == BLACK && w->left->color == BLACK) { // case 2
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->left->color == BLACK) { // case 3
                        w->right->color = BLACK;
                        w->color = RED;
                        leftRotate(w);
                        w = x->parent->left;
                    } // case 4
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = BLACK;
    }

    void inorder(Node* node) {
        if (node == NIL) return;
        inorder(node->left);
        std::cout << node->key << " ";
        inorder(node->right);
    }

    void print(const std::string& prefix, const Node* node, bool isLeft)
    {
        if( node != NIL )
        {
            const char* color = (node->color == RED ? RED_COLOR : BLACK_COLOR);
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
    RBT() : root(nullptr) {
        init_sentinel();
        root = NIL;
    }

    void insert(int k) {
        insert(root, k);
    }

    void remove(int key){ // page 324
        Node* z = root;
        while (z != NIL && z->key != key) {
            if (key < z->key) z = z->left;
            else z = z->right;
        }

        if (z == NIL)  return;

        Node* y = z;
        Color yOriginalColor = y->color;
        Node* x = NIL;

        if (z->left == NIL) {
            x = z->right;
            transplant(z, z->right);
        }
        else if (z->right == NIL) {
            x = z->left;
            transplant(z, z->left);
        }
        else {
            // z has 2 children
            y = minimum(z->right);
            yOriginalColor = y->color;
            x = y->right;

            if (y->parent == z) {
                x->parent = y;
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }

            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }

        if (yOriginalColor == BLACK) {
            deleteFixup(x);
        }

        delete z;
    }

    void inorder() {
        inorder(root);
        std::cout << "\n";
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
    RBT tree;
    const int n = 20;
    
    //test 1
    std::cout << "INSERTING 1, 2, ..., " << n << " AND DELETING RANDOM PERM OF 1, 2, ..., " << n <<"\n";
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
    std::cout << "\n\nINSERTING RANDOM PERM OF 1, 2, ..., " << n << " AND THEN DELETING RANDOM PERM OF 1, 2, ..., " << n << "\n";
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