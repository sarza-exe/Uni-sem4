//
// Created by Sara on 19.03.2025.
//
#include <iostream>
#include <fstream>
#include <random>
using namespace std;

int comparisons = 0;
int swaps = 0;
bool showSteps = false;
int n;

bool isBigger(const int a, const int b) {
    comparisons++;
    return a > b;
}

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

int partition(int *array, int low, int high) //Hoare Partition
{
    int pivot = array[low];
    int i = low-1;
    int j = high+1;

    while(true)
    {
        do{ // from left find element >= than pivot
            i++;
        } while(isBigger(pivot, array[i])); //3 1 2 5 6 7 

        do{ // from right find element <= than pivot
            j--;
        } while(isBigger(array[j], pivot)); 

        if(i >= j) break;

        swapElements(array, i, j);
    }

    if(showSteps) display(array);
    return j;
}

void QuickSort(int *array, const int l, const int h) 
{
    if (l < h)
    {
        int pivot = partition(array, l, h);

        QuickSort(array, l, pivot);
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
    cout << "Number of swaps: " << swaps << "\n";

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
            swaps = 0;
            for (int j = 0; j < i; j++) {
                arr[j] = bin(rng);
            }
            QuickSort(arr, 0, i-1);
            file << comparisons << " " << swaps << " ";
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
            swaps = 0;
            for (int j = 0; j < i; j++) {
                arr[j] = bin(rng);
            }
            QuickSort(arr, 0, i-1);
            file2 << comparisons << " " << swaps << " ";
        }   
        file2 << "\n";  
    }
        
    file.close();
    file2.close();*/

    return 0;
}