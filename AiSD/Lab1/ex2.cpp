//
// Created by Sara on 13.03.2025.
//
#include <iostream>
#include <random>
#include "cyclic.h"
using namespace std;

void Insert(CyclicList *list, const int& value) {
    Node *newNode = new Node(value);

    if (list->head == nullptr) {
        newNode->next = newNode;
        list->head = newNode;
    }
    else {
        newNode->next = list->head->next;
        list->head->next = newNode;
    }
    list->size++;
}

void Merge(CyclicList *list1, CyclicList *list2) {
    if (list2->head == nullptr) return;
    Node *curr = list2->head;
    do
    {
        Insert(list1, curr->data);
        curr = curr->next;
    } while (curr != list2->head);
}

void Display(CyclicList* list) {
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

int Search(CyclicList* list, int value) {
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
        curr = curr->next;
    } while (curr != list->head);
    return comparisons;
}

int main() {
    CyclicList cyclic, cyclic2;

    for (int i = 1; i <= 10; i++) {
        Insert(&cyclic, i);
        Insert(&cyclic2, i+10);
    }

    Merge(&cyclic, &cyclic2);

    cout << "Merge of lists [1, ..., 10] and [11, ... 20] \n";
    Display(&cyclic);


    random_device rd;
    mt19937  rng(rd());
    uniform_int_distribution<int> bin(0, 100000);
    uniform_int_distribution<int> bin2(0, 9999);

    int array[10000];
    CyclicList list;
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
