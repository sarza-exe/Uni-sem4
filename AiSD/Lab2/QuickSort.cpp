//
// Created by Sara on 19.03.2025.
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

int partition(int *array, int low, int high)
{
    int pivot = array[high] ;
    int i ,j;
    j = low;

    for (int i = low; i < high; i++)
    {
        if(isBigger(pivot, array[i]))
        {
            move(array, i, j);
            j += 1;
        }
    }

    move(array, j, high);
    if(showSteps) display(array);

    return j; 
}

void QuickSort(int *array, const int l, const int h) 
{
    if (l < h)
    {
        int pivot = partition(array, l, h);

        QuickSort(array, l, pivot-1);
        QuickSort(array, pivot+1, h);
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
    

    /*
    //code for saving data
    showSteps = false;
    std::ofstream file("data/quickSortData.txt"); 
    std::ofstream file2("data/quickSortDataBig.txt");

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
            QuickSort(arr, 0, i-1);
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
            QuickSort(arr, 0, i-1);
            file2 << comparisons << " " << moves << " ";
        }   
        file2 << "\n";  
    }
        
    file.close();
    file2.close();*/

    return 0;
}