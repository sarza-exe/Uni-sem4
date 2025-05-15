//
// Created by Sara on 19.03.2025.
//

#include <iostream>
#include <random>
using namespace std;

int main(int argc, const char * argv[]) {
    if (argc != 2) {
        cerr << "Needed one argument. Got: " << argc-1 << endl;
        return -1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        cerr << "Needed number must be greater than 0" << endl;
    }

    cout << n << " ";
    for (int i = 0; i < n; i++) {
        cout << i << " ";
    }

    return 0;
}
