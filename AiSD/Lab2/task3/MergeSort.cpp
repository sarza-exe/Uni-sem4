//
// Created by Sara on 21.03.2025.
//
#include <iostream>
#include <fstream>
#include <random>
#include <vector>
using namespace std;

int comparisons = 0;
int moves = 0;
bool showSteps = false;
int n;

bool isBigger(const int a, const int b) {
    comparisons++;
    return a > b;
}

void move(int *array, const int i, const int value) {
    moves ++;
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

void Merge(int *arr, int low, int mid, int high)
{
    vector<int> temp; // temporary array
    int left = low;      // starting index of left half of arr
    int right = mid + 1;   // starting index of right half of arr

    //storing elements in the temporary array in a sorted manner
    while (left <= mid && right <= high) {
        moves++;
        if(isBigger(arr[left], arr[right])){
            temp.push_back(arr[right]);
            right++;
        }
        else{
            temp.push_back(arr[left]);
            left++;
        }
    }

    // if elements on the left half are still left
    while (left <= mid) {
        moves++;
        temp.push_back(arr[left]);
        left++;
    }

    //  if elements on the right half are still left
    while (right <= high) {
        moves++;
        temp.push_back(arr[right]);
        right++;
    }

    // transfering all elements from temporary to arr
    for (int i = low; i <= high; i++) {
        move(arr, i, temp[i-low]);
    }

    if(showSteps) display(arr);
}

void MergeSort(int *array, const int low, const int high) 
{
    if (low >= high) return;
    int mid = (low + high) / 2 ;
    MergeSort(array, low, mid);  // left half
    MergeSort(array, mid + 1, high); // right half
    Merge(array, low, mid, high);  // merging sorted halves
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

    MergeSort(array, 0, n-1);

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
    std::ofstream file("data/mergeSortData.txt");
    std::ofstream file2("data/mergeSortDataBig.txt");

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
            MergeSort(arr, 0, i-1);
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
            MergeSort(arr, 0, i-1);
            file2 << comparisons << " " << moves << " ";
        }   
        file2 << "\n";  
    }
        
    file.close();
    file2.close();
    */

    return 0;
}