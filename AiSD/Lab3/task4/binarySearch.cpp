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
        comparisons++;
        if(midValue == value) 
            return true;
        comparisons++;
        if(midValue > value) 
        {
            return BinarySearch(array, l, mid-1, value);
        }
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
        if((iterativeSearch(array, k) && stat) || (!iterativeSearch(array,k) && !stat)) cout << "Binary Search succeded!\n";
        else cout << "Binary Search failed\n";
    }

    cout << "Number of comparisons: " << comparisons << "\n";

    return 0;
}