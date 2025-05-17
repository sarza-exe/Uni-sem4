#include <iostream>
using namespace std;

enum Color {RED, BLACK};

struct Node{
    int key;
    Color color;
    Node* left;
    Node* right;
    Node* parent;
    Node(int value): key(value), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

class RBT{
private:
    Node* root;
    
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
                    //leftRotate(z);
                }
                else{
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    //rightRotate(z->parent->parent);
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
                    //rightRotate(z);
                }
                else{
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    //leftRotate(z->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }

    Node* insert(Node* node, int k) {
        Node* y = nullptr;
        Node* x = root;
        Node* z = new Node(k);
        while (x != nullptr)
        {
            y = x;
            if(z->key < x->key)
                x = x->left;
            else x = x->right;
        }
        z->parent = y;
        if(y == nullptr)
            root = z;
        else if (z->key < y->key)
            y->left = z;
        else y->right = z;
        insertFixup(z);
    }

public:
    // constructor
    RBT() : root(nullptr) {}

    void insert(int k) {
        root = insert(root, k);
    }

};