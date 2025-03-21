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

    
    /*
    //code for saving data
    showSteps = false;
    std::ofstream file("insertSortData.txt");

    if (!file) {
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
            InsertSort(arr, i);
            file << comparisons << " " << moves << " ";
        }   
        file << "\n";  
    }
        
    file.close();
    */

    return 0;
}