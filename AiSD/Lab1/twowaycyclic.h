//
// Created by Sara on 15.03.2025.
//

#ifndef TWOWAYCYCLIC_H
#define TWOWAYCYCLIC_H


struct Node {
    int data;
    Node* next;
    Node* prev;
    explicit Node(const int& data): data(data), next(nullptr), prev(nullptr) {}
};

struct TwoWayCyclic {
    Node* head;
    int size;

    TwoWayCyclic(): head(nullptr), size(0) {}

    ~TwoWayCyclic() {
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

#endif //TWOWAYCYCLIC_H
