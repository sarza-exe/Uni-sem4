//
// Created by Sara on 25.04.2025.
//
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>    // std::sort
using namespace std;

int comparisons = 0;
int swaps = 0;
bool showSteps = false;
int n;

void swapElements(int *array, const int i, const int j) {
    swaps += 3;
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

random_device rd;
mt19937 rng(rd());

int RandomizedPartitionLomuto(int *array, int p, int r) {
    uniform_int_distribution<int> dist(p, r);    
    int pivotIdx = dist(rng);
    if(showSteps) cout << "Pivot: " << array[pivotIdx] << "\t";

    swapElements(array, pivotIdx, r);
    int pivot = array[r];
    int i = p - 1;
    for (int j = p; j < r; ++j) {
        comparisons++;
        if (array[j] <= pivot) {
            swapElements(array, ++i, j);
        }
    }
    swapElements(array, i+1, r);
    if(showSteps) display(array);
    return i + 1;
}

int RandomizedSelect(int *array, const int p, const int r, const int i) 
{
    if (p == r)
        return array[p];
    int q = RandomizedPartitionLomuto(array, p, r);
    int k = q - p + 1;
    if (i == k)
        return array[q];
    else if (i < k)
        return RandomizedSelect(array, p, q-1, i);
    return RandomizedSelect(array, q+1, r, i-k);
}


int main() 
{
    cin >> n;
    if(n <= 30) showSteps = true;

    int k;
    cin >> k;

    int array[n];
    int copy[n];
    for(int i = 0; i < n; i++)
    {
        cin >> array[i];
        if(showSteps) copy[i] = array[i];
    }

    int stat = RandomizedSelect(array, 0, n-1, k);
    // .\data_random 20 | .\randomSelect 

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
    // std::ofstream file1("data/randomSelectData1.txt"); 
    // std::ofstream file2("data/randomSelectDataquatern.txt"); 
    // std::ofstream file3("data/randomSelectDatanhalfn.txt"); 

    // if (!file1 && !file2 && !file3) {
    //     std::cerr << "Error opening file!" << std::endl;
    //     return -1;
    // }

    // random_device rd;
    // mt19937 rng(rd());

    // for(int i = 100; i <= 50000; i += 100)
    // {
    //     int arr[i], cp1[i], cp2[i];
    //     int k1 = 1;
    //     int k2 = i/4;
    //     int k3 = i/2;
    //     uniform_int_distribution<int> bin(0, 2*i-1);
    //     uniform_int_distribution<int> bin2(1, i);
    //     for(int m = 0; m < 50; m++) // repeat 50 times
    //     {
    //         for (int j = 0; j < i; j++) {
    //             arr[j] = bin(rng);
    //             cp1[j] = arr[j];
    //             cp2[j] = arr[j];
    //         }
    //         comparisons = 0;
    //         swaps = 0;
    //         int stat1 = RandomizedSelect(arr, 0, i-1, k1);
    //         file1 << comparisons << " " << swaps << " ";

    //         comparisons = 0;
    //         swaps = 0;
    //         int stat2 = RandomizedSelect(cp1, 0, i-1, k2);
    //         file2 << comparisons << " " << swaps << " ";

    //         comparisons = 0;
    //         swaps = 0;
    //         int stat3 = RandomizedSelect(cp2, 0, i-1, k3);
    //         file3 << comparisons << " " << swaps << " ";
    //     }   
    //     file1 << "\n"; 
    //     file2 << "\n"; 
    //     file3 << "\n";  
    // }
        
    // file1.close();
    // file2.close();
    // file3.close();

    return 0;
}