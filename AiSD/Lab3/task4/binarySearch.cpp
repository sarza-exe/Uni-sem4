//
// Created by Sara on 25.04.2025.
//
#include <iostream>
#include <fstream>
#include <random>
#include <cmath> //ceil
using namespace std;

int comparisons = 0;
bool showSteps = false;
int n;

void display(int *array)
{
    for (int i=0; i<n; i++) {
        int key = array[i];
        if(key < 10) cout << "0";
        cout << key << " ";
    }
    cout << endl;
}


bool BinarySearch(int *array, const int l, const int r, const int value) 
{
    if(r >= l)
    {
        int mid = l+(r-l)/2;
        int midValue = array[mid];
        if(midValue == value) {
            comparisons++;
            return true;
        }
        if(midValue > value) 
        {
            comparisons++;
            cout << l << " " << mid-1 << endl;
            return BinarySearch(array, l, mid-1, value);
        }
        cout << mid+1 << " " << r << endl;
        return BinarySearch(array, mid+1, r, value); 
    }
    return false;
}

bool iterativeSearch(int *array, const int value)
{
    for(int i = 0; i < n; i++)
        if(array[i] == value) return true;
    return false;
}

int main() 
{
    int k;

    cin >> n;
    cin >> k;

    if(n <= 30) showSteps = true;

    int array[n];

    for(int i = 0; i < n; i++)
        cin >> array[i];

    bool stat = BinarySearch(array, 0, n-1, k);
    // .\data_random 20 | .\BinarySearch 

    if(showSteps)
    {
        cout << "Array:\n";
        display(array);
        cout << "Searched for: " << k << "\n";
        if((iterativeSearch(array, k) && stat) || (!iterativeSearch(array,k) && !stat)) cout << "Success!\n";
        else cout << "Failure\n";
    }

    cout << "Number of comparisons: " << comparisons << "\n";

    // if(array[k-1] != stat)
    // {
    //     cout << "Failed to found " << k << "-th smallest element.\n";
    //     return -1;
    // }
    // cout << "Managed to find " << k << "-th smallest element.\n";

    
    // //code for saving data
    // showSteps = false;
    // std::ofstream file1("data/binarySearchData1.txt"); 
    // std::ofstream file2("data/binarySearchDataquatern.txt"); 
    // std::ofstream file3("data/binarySearchDatanhalfn.txt"); 

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
    //         int stat1 = BinarySearch(arr, 0, i-1, k1);
    //         file1 << comparisons << " " << swaps << " ";

    //         comparisons = 0;
    //         swaps = 0;
    //         int stat2 = BinarySearch(cp1, 0, i-1, k2);
    //         file2 << comparisons << " " << swaps << " ";

    //         comparisons = 0;
    //         swaps = 0;
    //         int stat3 = BinarySearch(cp2, 0, i-1, k3);
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