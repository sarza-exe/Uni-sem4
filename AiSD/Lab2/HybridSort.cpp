//
// Created by Sara on 20.03.2025.
//
#include <iostream>
#include <fstream>
#include <random>
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

int partition(int *array, int low, int high)
{
    int pivot = array[high] ;
    int i ,j;
    j = low;

    for (int i = low; i < high; i++)
    {
        if(isBigger(pivot, array[i]))
        {
            swapPos(array, i, j);
            j += 1;
        }
    }

    swapPos(array, j, high);
    if(showSteps) display(array);

    return j; 
}

void HybridSort(int *array, const int l, const int h) 
{
    if ((h-l) > 17)
    {
        int pivot = partition(array, l, h);

        HybridSort(array, l, pivot-1);
        HybridSort(array, pivot+1, h);
    }
    else // for n <= 18
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

    HybridSort(array, 0, n-1);

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



    /*
    //Code for saving data
    showSteps = false;
    std::ofstream file("hybridSortData.txt"); // Open in append mode
    std::ofstream file2("hybridSortDataBig.txt");

    if (!file || !file2) {
        std::cerr << "Error opening file!" << std::endl;
        return -1;
    }

    random_device rd;
    mt19937 rng(rd());

    for(int i = 10; i <= 50; i += 10)
    {
        int arr[i];
        uniform_int_distribution<int> bin(0, 2*i-1);
        for(int k = 0; k < 100; k++) // repeat 100 times
        {
            comparisons = 0;
            moves = 0;
            for (int j = 0; j < i; j++) {
                arr[j] = bin(rng);
            }
            HybridSort(arr, 0, i-1);
            file << comparisons << " " << moves << " ";
        }   
        file << "\n";  
    }

    for(int i = 1000; i <= 50000; i += 1000)
    {
        int arr[i];
        uniform_int_distribution<int> bin(0, 2*i-1);
        for(int k = 0; k < 100; k++) // repeat 100 times
        {
            comparisons = 0;
            moves = 0;
            for (int j = 0; j < i; j++) {
                arr[j] = bin(rng);
            }
            HybridSort(arr, 0, i-1);
            file2 << comparisons << " " << moves << " ";
        }   
        file2 << "\n";  
    }
        
    file.close();
    file2.close();
    */
    
    return 0;
}