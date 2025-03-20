//
// Created by Sara on 20.03.2025.
//
#include <iostream>
using namespace std;

int comparisons = 0;
int moves = 0;
bool showSteps = false;
int n;

bool isBigger(const int a, const int b) {
    comparisons++;
    return a > b;
}

void swapPos(int *array, const int i, const int j) {
    moves += 3;
    swap(array[i], array[j]);
}

void moveInsert(int *array, const int i, const int value) {
    moves++;
    array[i] = value;
}

void display(int *array)
{
    for (int i=0; i<n; i++) {
        int key = array[i];
        if(key < 10) cout << "0";
        cout << key << " ";
    }
    cout << endl;
}

void InsertSort(int *array, const int l, const int h) {
    for (int i = l+1; i <= h; i++) {
        int key = array[i];
        int j = i - 1;
        while (j >= l && isBigger(array[j], key)) {
            moveInsert(array, j+1, array[j]);
            j--;
        }
        moveInsert(array, j+1, key);
        if(showSteps) display(array);
    }
}

int partition(int *array, const int l, const int h)
{
    int pivot = array[l];
    int i = h+1;

    for(int j = h; j > l; j--)
    {
        if(isBigger(array[j], pivot))
        {
            i--;
            swapPos(array, i, j);
        }
    }

    //move pivot after smaller elements
    swapPos(array, l, i - 1);
    display(array);
    return i - 1;

}

void QuickSort(int *array, const int l, const int h) 
{
    if ((h-l) > 11)
    {
        int pivot = partition(array, l, h);

        QuickSort(array, l, pivot-1);
        QuickSort(array, pivot+1, h);
    }
    else // for n <= 12
    {
        InsertSort(array, l, h);
    }
}


int main() 
{
    cin >> n;

    if(n < 40) showSteps = true;

    int array[n];
    int copy[n];
    for(int i = 0; i < n; i++)
    {
        cin >> array[i];
        if(showSteps) copy[i] = array[i];
    }

    if(showSteps) display(array);

    QuickSort(array, 0, n-1);

    if(showSteps)
    {
        cout << "Input Array:\n";
        display(copy);
        cout << "Sorted Array:\n";
        display(array);
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