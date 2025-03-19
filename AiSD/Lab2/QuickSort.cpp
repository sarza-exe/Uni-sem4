//
// Created by Sara on 19.03.2025.
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

void move(int *array, const int i, const int j) {
    moves += 3;
    swap(array[i], array[j]);
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

int partition(int *array, const int k, const int l)
{
    int pivot = array[k];
    int i = l+1;

    for(int j = l; j > k; j--)
    {
        if(isBigger(array[j], pivot))
        {
            i--;
            move(array, i, j);
        }
    }

    //move pivot after smaller elements
    move(array, k, i - 1);
    display(array);
    return i - 1;

}

void QuickSort(int *array, const int k, const int l) 
{
    if (k < l)
    {
        int pivot = partition(array, k, l);

        QuickSort(array, k, pivot-1);
        QuickSort(array, pivot+1, l);
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