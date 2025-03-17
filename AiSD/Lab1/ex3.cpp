//
// Created by Sara on 13.03.2025.
//
#include <iostream>
#include <random>
#include "twowaycyclic.h"
using namespace std;

void Insert(TwoWayCyclic *list, const int& value) {
    Node *newNode = new Node(value);

    if (list->head == nullptr) {
        newNode->next = newNode;
        newNode->prev = newNode;
        list->head = newNode;
    }
    else {
        Node* tail = list->head->prev;
        newNode->next = list->head;
        newNode->prev = tail;
        tail->next = newNode;
        list->head->prev = newNode;
    }
    list->size++;
}

void Merge(TwoWayCyclic *list1, TwoWayCyclic *list2) {
    if (list2->head == nullptr) return;
    Node *curr = list2->head;
    do
    {
        Insert(list1, curr->data);
        curr = curr->next;
    } while (curr != list2->head);
}

void Display(TwoWayCyclic* list) {
    if (list->head == nullptr) {
        cout << "List is empty.\n";
        return;
    }

    Node* temp = list->head;
    do {
        cout<<temp->data<<" -> ";
        temp = temp->next;
    } while (temp != list->head);
    cout << "\n";
}

int Search(TwoWayCyclic* list, int value) {
    random_device rd;
    mt19937  rng(rd());
    uniform_int_distribution<int> bin(0, 1);
    const int dir = bin(rng);

    int comparisons = 0;
    if (list->head == nullptr) {
        cout << "List is empty.\n";
        return 0;
    }

    Node* curr = list->head;
    do {
        comparisons++;
        if (curr->data == value) {
            return comparisons;
        }
        if (dir == 1) {curr = curr->next;}
        else {curr = curr->prev;}
    } while (curr != list->head);
    return comparisons;
}

int main() {
    TwoWayCyclic cyclic, cyclic2;

    for (int i = 20; i < 30; i++) {
        Insert(&cyclic, i);
        Insert(&cyclic2, i+10);
    }
    cout << "List1: ";
    Display(&cyclic);
    cout << "List2: ";
    Display(&cyclic2);

    Merge(&cyclic, &cyclic2);

    cout << "Merge of list1 and list2\n";
    Display(&cyclic);


    random_device rd;
    mt19937  rng(rd());
    uniform_int_distribution<int> bin(0, 100000);
    uniform_int_distribution<int> bin2(0, 9999);

    int array[10000];
    TwoWayCyclic list;
    for (int & i : array) {
        i = bin(rng); // insert random number [0, 100000] into array
        Insert(&list, i); // insert random number to cyclic list
    }

    int totalComparisonsI = 0; // random numbers in range <0,100000>
    int totalComparisonsT = 0; // random numbers from array

    for (int i = 0; i < 1000; i++)
    {
        const int valueI = bin(rng);
        totalComparisonsI += Search(&list, valueI);

        const int index = bin2(rng);
        totalComparisonsT += Search(&list, array[index]);
    }

    int average = totalComparisonsI / 1000;
    cout << "Average comparisons for random values in range <0, 100000>: " << average << "\n";

    average = totalComparisonsT / 1000;
    cout << "Average comparisons for random values from array: " << average << "\n";

    return 0;
}
