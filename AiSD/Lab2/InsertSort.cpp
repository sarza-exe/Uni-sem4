//
// Created by Sara on 19.03.2025.
//
#include <iostream>
using namespace std;

/*
● INSERTION SORT ,
● QUICK SORT ,
● Algorytm hybrydowy: QUICK SORT, który dla małych podtablic przełącza się na
INSERTION SORT.

● Dla rozmiaru danych n < 40:
○ stany sortowanej tablicy w istotnych momentach (np. w MERGE SORT - po
zakończeniu każdego scalania),

● Dla dowolnego rozmiaru danych, na końcu:
○ łączną liczbę porównań między kluczami,
○ łączną liczbę przestawień kluczy.
(Zaimplementować osobne funkcje/procedury do porównywania i
przestawiania kluczy, które dodatkowo zwiększają swój globalny licznik
odpowiednio porównań lub przestawień.)

Finalnie, program sam sprawdza, czy wynikowy ciąg jest posortowanym ciągiem
wejściowym.
W prezentacji zademonstrować testy dla długości tablicy n ( n ∈ {8, 32}) dla ciągów:
● losowego,
● posortowanego malejąco,
● posortowanego rosnąco.
*/

int comparisons = 0;
int moves = 0;
bool showSteps = false;

bool isBigger(const int a, const int b) {
    comparisons++;
    return a > b;
}

void move(int *array, const int i, const int value) {
    moves++;
    array[i] = value;
}

void display(int *array, int n)
{
    for (int i=0; i<n; i++) {
        int key = array[i];
        if(key < 10) cout << "0";
        cout << key << " ";
    }
    cout << endl;
}

void InsertSort(int *array, const int n) {
    for (int i = 1; i < n; i++) {
        int key = array[i];
        int j = i - 1;
        while (j >= 0 && isBigger(array[j], key)) {
            move(array, j+1, array[j]);
            j--;
        }
        move(array, j+1, key);
        if(showSteps) display(array, n);
    }
}

int main() 
{
    int n;
    cin >> n;

    if(n < 40) showSteps = true;

    int array[n];
    int copy[n];
    for(int i = 0; i < n; i++)
    {
        cin >> array[i];
        if(showSteps) copy[i] = array[i];
    }

    if(showSteps) display(array, n);

    InsertSort(array, n);

    if(showSteps)
    {
        cout << "Input Array:\n";
        display(copy, n);
        cout << "Sorted Array:\n";
        display(array, n);
    }

    cout << "Number of comparisons: " << comparisons << "\n";
    cout << "Number of swaps: " << moves << "\n";

    for(int i = 0; i < n-1; i++)
    {
        if(array[i] > array[i+1])
        {
            cout << "Array is not sorted.\n";
            return -1;
        }
    }

    cout << "Array is indeed sorted\n";
    
    return 0;
}