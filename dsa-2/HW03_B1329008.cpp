//heap sort ,insertion sort ,quicksort
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>

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

void insertionsort(int* arr, int n) {
    for(int i = 1; i<n;i++) {
        int temp = arr[i];
        int j = i-1;
        while(j>=0&&temp<arr[j]){
            arr[j+1] = arr[j];
            j--;
        }
        arr[j+1] = temp;
    }
}

void genarr (int* arr, int n) {//陣列的第0格不用, n是幾就生幾個數字
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(1, 50000);
    for (int i = 1;i <= n ;i++) {
        arr[i] = distrib(gen);
    }
}

void manual(int* arr, int n) {
    int choose;
    cout << "1.HeapSort, 2.InsertionSort, 3.QickSort:";
    cin >> choose;
    if (choose == 1) {
        heapsort(arr, n);
        for(int i =1; i <=n; i++) {
            cout << arr[i] << " ";
        }
    }
    else if (choose == 2) {
        insertion_sort(arr, n);
        for(int i =1; i <=n; i++) {
            cout << arr[i] << " ";
        }
    }
    else if (choose == 3) {
        quicksort(arr, n);
        for(int i =1; i <=n; i++) {
            cout << arr[i] << " ";
        }
    }
    else{
        cout << "沒這個";
    }
}

int main() {
    int m;
    cout << "1.看結果的, 2.實驗那個";
    cin >> m;

    if(m == 1) {
        int n;
        cout << "要幾個值?";
        cin >> n;
        int arr[n+1];
        genarr(arr, n);
        manual(arr, n);
    }
    else if (m == 2) {
        int list[5] = {10, 100, 1000, 10000, 100000};

        for (int i=0; i<5; i++ ) {
            double total_time = 0;
            for(int j = 0;j<100;j++) {
                int arr[list[i]+1];
                genarr(arr,list[i]);
                
                auto start = chrono::high_resolution_clock::now();
            
                //quicksort(arr, 1, list[i]);
                heapsort(arr, list[i]);
                //insertion_sort(arr, list[i]);
                //insertionsort(arr, list[i]);
                
                auto end = chrono::high_resolution_clock::now();
                auto ns_int = chrono::duration_cast<chrono::nanoseconds>(end - start);
                total_time += ns_int.count();
            }
            cout << (total_time/100) << ",單位:10^-9秒" << endl;
        }
    }
    else{
        cout << "0.0";
    }

    return 0;
}