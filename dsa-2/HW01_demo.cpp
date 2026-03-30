#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
#include <vector>

using namespace std;

void adjust(int* arr, const int root, const int n) {
    int e = arr[root];
    int j;
    for(j = 2*root;j <= n;j*=2) {
        if(j < n&& arr[j] < arr[j+1]) {j++;}
        if(e >= arr[j]){break;}
        arr[j/2] = arr[j];
    }
    arr[j/2] = e;
}

void heapsort(int* arr, const int n) {
    for(int i = n/2; i >= 1; i--) {
        adjust(arr, i, n);
    }
    for(int i = n-1;i >= 1; i--) {
        swap(arr[1], arr[i+1]);
        adjust(arr, 1, i);
    }
}

void quick_sort(int* a, const int left, const int right) {
    if(left < right) {
        int i = left;
        int j = right +1;
        int pivot = a[left];
        do {
            do i++;while(pivot > a[i] && i <= right);
            do j--; while(pivot < a[j] && j >= left);
            if(i < j) {
                swap(a[i], a[j]);
            }
        }while(i < j);
        swap(a[left], a[j]);

        quick_sort(a, left, j-1);
        quick_sort(a, j+1, right);
    }
}

void quicksort(int* arr, const int n) {
    quick_sort(arr, 1, n);
}

void insert (const int& e, int* arr, int i) {
    arr[0] = e;
    while (e < arr[i]) {
        arr[i +1] = arr[i];
        i--;
    }
    arr[i+1] = e;
}

void insertion_sort(int* arr, const int n) {
    for(int j = 2;j <= n;j++) {
        int temp = arr[j];
        insert(temp, arr, j-1);
    }
}

void genarr (int* arr, int n) {//陣列的第0格不用
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(1, 50000);
    for (int i = 1;i <= n ;i++) {
        arr[i] = distrib(gen);
    }
}

int main() {

    int n = 5;
    int arr[n+1];
    genarr (arr, n);

    quicksort(arr, n);
    //heapsort(arr, n);
    //insertion_sort(arr, n);

    for(int i=1; i<6; i++) {
        cout << arr[i] << " ";
    }

    return 0;
}