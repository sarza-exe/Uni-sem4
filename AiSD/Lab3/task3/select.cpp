//
// Created by Sara on 25.04.2025.
//
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>    // std::sort
#include <cmath>     //ceil
using namespace std;

int comparisons = 0;
int swaps = 0;
bool showSteps = false;
int n;

void swapElements(int *array, const int i, const int j) {
    swaps += 3;
    swap(array[i], array[j]);
}

bool isBigger(const int a, const int b) {
    comparisons++;
    return a > b;
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

void InsertSort(int *array, const int l, const int h, bool print) {
    for (int i = l+1; i <= h; i++) {
        int key = array[i];
        int j = i - 1;
        while (j >= l && isBigger(array[j], key)) {
            array[j+1] = array[j];
            swaps++;
            j--;
        }
        array[j+1] = key;
        swaps++;
    }
    if(showSteps && print) display(array);
}

int Partition(int *array, int l, int r, int x) {
    // Find x in A[l..r] and move it to end
    int pivotIndex = l;
    while (pivotIndex <= r && array[pivotIndex] != x) pivotIndex++;
    swapElements(array, pivotIndex, r);
    int pivot = array[r];

    int i = l - 1;
    for (int j = l; j < r; ++j) {
        // array[j] <= pivot
        if (!isBigger(array[j], pivot)) {
            i++;
            swapElements(array, i, j);
        }
    }
    swapElements(array, i + 1, r);
    return i + 1;
}

int Select(int *array, const int p, const int r, const int i, const int blockSize) 
{
    if (p == r) return array[p];

    int length = r - p + 1;
    // if there are <= blockSize than we can just sort it cuz it's like finding a median
    if(length <= blockSize){
        InsertSort(array, p, r, false);
        return array[p + i - 1];
    }

    int sizeOfMedians = ceil(length*1.0/blockSize);
    int *medians = new int[sizeOfMedians];

    //Find the median for each ceil(n/blockSize) groups
    for (int g = 0; g < sizeOfMedians; g++) {
        int gl = p + g*blockSize;
        int gr = min(gl + blockSize - 1, r);
        InsertSort(array, gl, gr, false);  // sort each 5 element block
        medians[g] = array[ gl + (gr - gl)/2 ]; // pick its median
    }

    int mom = Select(medians, 0, sizeOfMedians-1, (sizeOfMedians+1)/2, blockSize);
    delete[] medians;
    int q = Partition(array, p, r, mom); 

    int k = q - p + 1;
    if (i == k)
        return array[q];
    else if (i < k)
        return Select(array, p, q-1, i, blockSize);
    return Select(array, q+1, r, i-k, blockSize);
}

int main() 
{
    int k;

    cin >> n;
    cin >> k;

    if(n <= 30) showSteps = true;

    int array[n];
    int copy[n];

    for(int i = 0; i < n; i++){
        cin >> array[i];
        if(showSteps) copy[i] = array[i];
    }

    int blockSize = 7;
    int stat = Select(array, 0, n-1, k, blockSize);
    // .\data_random 20 | .\select 

    if(showSteps)
    {
        cout << "End Array State:\n";
        display(array);
        cout << "Input Array:\n";
        display(copy);
        cout << k << "-th smallest element: " << stat << "\n";
        sort(array, array+n);
        cout << "Sorted Array:\n";
        display(array);
    }

    cout << "Number of comparisons: " << comparisons << "\n";
    cout << "Number of swaps: " << swaps << "\n";

    if(array[k-1] != stat)
    {
        cout << "Failed to found " << k << "-th smallest element.\n";
        return -1;
    }


    cout << "Managed to find " << k << "-th smallest element.\n";

    
    // //code for saving data
    // showSteps = false;
    // std::ofstream file("data/selectData.txt"); 

    // if (!file) {
    //     std::cerr << "Error opening file!" << std::endl;
    //     return -1;
    // }

    // random_device rd;
    // mt19937 rng(rd());

    // for(int i = 100; i <= 50000; i += 100)
    // {
    //     int arr[i];
    //     uniform_int_distribution<int> bin(0, 2*i-1);
    //     uniform_int_distribution<int> bin2(1, i);
    //     for(int m = 0; m < 100; m++) // repeat 100 times
    //     {
    //         comparisons = 0;
    //         swaps = 0;
    //         for (int j = 0; j < i; j++) {
    //             arr[j] = bin(rng);
    //         }
    //         int k = bin2(rng);
    //         int stat = Select(array, 0, i-1, k);
    //         file << comparisons << " " << swaps << " ";
    //     }   
    //     file << "\n";  
    // }
        
    // file.close();

    return 0;
}