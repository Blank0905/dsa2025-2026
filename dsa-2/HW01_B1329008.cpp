//heap sort ,insertion sort ,quicksort
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

void quicksort(int* a, const int left, const int right) {
    if(left < right) {
        int i = left;
        int j = right +1;
        int pivot = a[left];
        do {
            do i++;while(pivot > a[i]);
            do j--; while(pivot < a[j]);
            if(i < j) {
                swap(a[i], a[j]);
            }
        }while(i < j);
        swap(a[left], a[j]);

        quicksort(a, left, j-1);
        quicksort(a, j+1, right);
    }
}

void genarr (int* arr, int n) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(1, 50000);
    for (int i = 0;i < n ;i++) {
        arr[i] = distrib(gen);
    }
}

int main() {
    int n = 10000;
    int a[n];
    genarr(a, n);
    
    auto start = chrono::high_resolution_clock::now();
    
    quicksort(a, 0, n-1);
    
    auto end = chrono::high_resolution_clock::now();

    auto us_int = chrono::duration_cast<chrono::microseconds>(end - start);

    cout << us_int.count() << "微秒" << endl;

    // for(int i =0; i <5; i++) {
    //     cout << a[i] << " ";
    // }

    // for(int i =n-5; i <n; i++) {
    //     cout << a[i] << " ";
    // }



    return 0;
}