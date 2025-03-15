//
// Created by Sara on 15.03.2025.
//

#ifndef CYCLIC_H
#define CYCLIC_H

struct Node {
    int data;
    Node *next;
    explicit Node(const int& value) : data(value), next(nullptr) {}
};

struct CyclicList {
    int size;
    Node *head;
    CyclicList() : size(0), head(nullptr) {}

    ~CyclicList() {
        if (head == nullptr) return;
        Node* curr = head;

        do {
            Node *nextNode = curr->next;
            delete curr;
            curr = nextNode;
        } while (curr != head);

        head = nullptr;
    }
};

#endif //CYCLIC_H
