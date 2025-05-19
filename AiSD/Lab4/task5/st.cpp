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

    int height_iterative() {
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

    void splay(Node* x){

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

        if (root == nullptr) root = z;

        Node *p = nullptr, *curr = root;
        while (curr != nullptr) {
            p = curr;
            if (curr->key > x)
                curr = curr->left;
            else if (curr->key < x){
                curr = curr->right;
            }
            else return;
        }
        z->parent = p;

        if (p->key > x)
            p->left = z;
        else
            p->right = z;
        
        splay(z);
    }

    void remove(int key){
        Node* curr = find(key);
        if (!curr) return;

        Node* parent;  //remember parent of curr
        if (!curr->left || !curr->right) {
            // at most one child
            Node* x = (curr->left ? curr->left : curr->right);
            transplant(curr, x);
        } else {
            // two children -> find successor
            Node* succ = curr->right;
            while (succ->left) succ = succ->left;

            if (succ->parent != curr) {
                // detach succ, replace with its right child
                transplant(succ, succ->right);
                // hook curr->right under succ
                succ->right = curr->right;
                succ->right->parent = succ;
            }
            // now replace curr with succ
            transplant(curr, succ);
            succ->left = curr->left;
            succ->left->parent = succ;
        }
        delete curr;
        if(parent) splay(parent);
    }

    int height() {
        return height_iterative();
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


    //test 2
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