//
// Created by Sara on 6.04.2025.
//
#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <queue>
using namespace std;

int comparisons = 0;
int moves = 0;
bool showSteps = false;
int n;

bool isBigger(const int a, const int b) {
    comparisons++;
    return a > b;
}

void display(const std::vector<int>& array)
{
    for (int i=0; i<array.size(); i++) {
        int key = array[i];
        if(key < 10) cout << "0";
        cout << key << " ";
    }
    cout << endl;
}

// Merge two sorted vectors into one sorted vector
std::vector<int> merge(const std::vector<int>& left, const std::vector<int>& right) {
    std::vector<int> result;
    int i = 0, j = 0;
    // Reserve space to avoid repeated reallocation
    result.reserve(left.size() + right.size());

    while (i < left.size() && j < right.size()) {
        comparisons++;
        moves++;
        if (left[i] <= right[j]) {
            result.push_back(left[i]);
            i++;
        } else {
            result.push_back(right[j]);
            j++;
        }
    }
    // Append remaining elements from left and right
    while (i < left.size()) {
        moves++;
        result.push_back(left[i]);
        i++;
    }
    while (j < right.size()) {
        moves++;
        result.push_back(right[j]);
        j++;
    }
    return result;
}

// Structure to hold a run for the priority queue.
struct Run {
    std::vector<int> data;
    int id;  // unique id to break ties

    Run(const std::vector<int>& d, int i) : data(d), id(i) {}
};

// Comparator for the minheap based on run length.
struct RunComparator {
    bool operator()(const Run& a, const Run& b) const {
        // We want the run with the smaller size to have higher priority.
        // If sizes are equal, compare by id.
        return (a.data.size() > b.data.size()) || 
               (a.data.size() == b.data.size() && a.id > b.id);
    }
};

// Merge runs in an optimal order using a priority queue
std::vector<int> MergeOptimal(const std::vector< std::vector<int> >& runs)
{
    // priority_queue< T - stored type, contrainer, Compare type providing a strict weak ordering.> 
    std::priority_queue<Run, std::vector<Run>, RunComparator> minHeap;
    int unique_id = 0;

    // Insert all runs into the heap
    for (const auto& run : runs) {
        minHeap.emplace(run, unique_id++);
    }

    // While more than one run remains, merge the two smallest runs.
    while (minHeap.size() > 1) {
        Run run1 = minHeap.top();
        minHeap.pop();
        Run run2 = minHeap.top();
        minHeap.pop();
        
        std::vector<int> merged = merge(run1.data, run2.data);
        if(showSteps) display(merged);
        // Push the merged run back into the heap
        minHeap.emplace(merged, unique_id++);
    }
    return minHeap.top().data;
}

// Find ascending runs in the array
std::vector<std::vector<int>> FindRuns(const std::vector<int>& array) {
    std::vector<std::vector<int>> runs;
    if (array.empty())
        return runs;

    // add size of array to moves because we assign every element to sume run
    moves += array.size();
    
    int runStart = 0;
    for (int i = 1; i < array.size(); ++i) {
        if (isBigger(array[i-1], array[i])) { 
            // Found end of a run; copy run [runStart, i-1)
            runs.emplace_back(array.begin() + runStart, array.begin() + i);
            runStart = i;
        }
    }
    // Add the last run omited in for
    runs.emplace_back(array.begin() + runStart, array.end());
    return runs;
}

/*
Divide - divide array into runs that have natural order
Conquer - merge runs by always choosing the two shortest runs
*/
std::vector<int> NewSort(const std::vector<int>& array)
{
    // Find arrays of ascending order
    std::vector<std::vector<int>> runs = FindRuns(array);
    if(runs.size() <= 1) //if the array is sorted - return
        return runs[0];
    return MergeOptimal(runs);
}


int main() 
{
    cin >> n;

    if(n < 40) showSteps = true;

    std::vector<int> array;
    std::vector<int> copy;
    for(int i = 0; i < n; i++)
    {
        int temp;
        cin >> temp;
        array.push_back(temp);
    }

    if(showSteps) {
        copy = array;
        display(array);
    }

    array = NewSort(array);

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
        if(array.at(i) > array.at(i+1))
        {
            cout << "Array is not sorted.\n";
            return -1;
        }
    }

    cout << "Array is indeed sorted\n";
    

    /*
    //code for saving data
    showSteps = false;
    std::ofstream file("data/newSortData.txt");
    std::ofstream file2("data/newSortDataBig.txt");

    if (!file || !file2) {
        std::cerr << "Error opening file!" << std::endl;
        return -1;
    }

    random_device rd;
    mt19937 rng(rd());

    for(int i = 10; i <= 50; i += 10)
    {
        uniform_int_distribution<int> bin(0, 2*i-1);
        for(int k = 0; k < 100; k++) // repeat 100 times
        {
            std::vector<int> arr;
            comparisons = 0;
            moves = 0;
            for (int j = 0; j < i; j++) {
                arr.push_back(bin(rng));
            }
            arr = NewSort(arr);
            file << comparisons << " " << moves << " ";
        }   
        file << "\n";  
    }

    for(int i = 1000; i <= 50000; i += 1000)
    {
        uniform_int_distribution<int> bin(0, 2*i-1);
        for(int k = 0; k < 100; k++) // repeat 100 times
        { 
            std::vector<int> arr;
            comparisons = 0;
            moves = 0;
            for (int j = 0; j < i; j++) {
                arr.push_back(bin(rng));
            }
            arr = NewSort(arr);
            file2 << comparisons << " " << moves << " ";
        }   
        file2 << "\n";  
    }
        
    file.close();
    file2.close();*/
    

    return 0;
}