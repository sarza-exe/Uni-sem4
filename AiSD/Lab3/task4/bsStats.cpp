//
// Created by Sara on 25.04.2025.
//
#include <iostream>
#include <fstream>
#include <random>
#include <cmath> //ceil
#include <chrono>
using namespace std;
using namespace std::chrono;

long long comparisons = 0;
bool showSteps = false;
int n;

bool BinarySearch(int *array, const int l, const int r, const int value) 
{
    if(r >= l)
    {
        int mid = l+(r-l)/2;
        int midValue = array[mid];
        comparisons++;
        if(midValue == value) {
            return true;
        }
        comparisons++;
        if(midValue > value) 
        {
            return BinarySearch(array, l, mid-1, value);
        }
        return BinarySearch(array, mid+1, r, value); 
    }
    return false;
}

int main() 
{
    
    //code for saving data
    showSteps = false;
    std::ofstream file1("data/bsRandom.txt"); 

    if (!file1) {
        std::cerr << "Error opening file!" << std::endl;
        return -1;
    }

    random_device rd;
    mt19937 rng(rd());

    for(int i = 1000; i <= 100000; i += 1000)
    {
        int arr[i];
        uniform_int_distribution<int> bin(0, 2*i-1);
        uniform_int_distribution<int> bin2(0, i-1);
        int val;

        comparisons = 0;
        auto start = high_resolution_clock::now();
        int reps = 200;
        int valIndex;
        //val = 2*i;
        for(int m = 0; m < reps; m++) // repeat
        {
            valIndex = bin2(rng);
            for (int j = 0; j < i; j++) {
                arr[j] = bin(rng);
                if (j == valIndex) val = arr[j];
            }
    
            bool find = BinarySearch(arr, 0, i-1, val);
        }   
        auto end = high_resolution_clock::now();
        auto duration_us = duration_cast<microseconds>(end - start).count();
        file1 << comparisons/reps << " " << duration_us/reps << "\n"; 
    }
        
    file1.close();

    return 0;
}