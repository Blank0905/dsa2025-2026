#include <iostream>
#include <vector>
#include <chrono>   // 用於計算時間
#include <random>   // 用於隨機生成
#include <algorithm> // 用於 sort

#include<random>
#include<set>

using namespace std;

// 定義節點結構
struct ListNode {
    int coef;
    int exp;
    ListNode *next;
    ListNode(int x, int y) {
        coef = x;
        exp = y;
        next = nullptr;
    }
};

// 將多項式操作封裝成類別
class Polynomial {
private:
    ListNode* head; // Circular linked list 的頭節點 (Dummy head)

public:
    Polynomial() {
        head = new ListNode(0, 0);
        head->next = head; // Circular
    }

    // 解構子：清除記憶體，但這東西超花時間
    /*~Polynomial() {
        clear();
        delete head;
    }*/

    void clear() {
        ListNode* current = head->next;
        while (current != head) {
            ListNode* temp = current;
            current = current->next;
            delete temp;
        }
        head->next = head;
    }

    // 加入一項 (插在 dummy head 後面)
    void addTerm(int coef, int exp) {
        ListNode* newNode = new ListNode(coef, exp);
        newNode->next = head->next;
        head->next = newNode;
    }
    
    // 生成 Dense 測試資料 (指數連續 0 ~ terms-1)
    void generateDense(int terms) {
        clear();
        for (int i = 0; i < terms; i++) {
            addTerm(1, i); 
        }
    }

    void generateSparse(int terms, std::mt19937& rng) {
        clear();
        std::uniform_int_distribution<int> dist(1, 1000);
        int currentExp = 0;
        for (int i = 0; i < terms; ++i) {
            currentExp += dist(rng); // 確保嚴格遞增
            addTerm(1, currentExp);
        }
    }

    // 核心乘法邏輯
    Polynomial multiply(const Polynomial& other) {
        Polynomial result;
        for(ListNode* a = head -> next; a != head ; a = a->next) {//遍歷A多項式
            ListNode* searchPtr = result.head;//讓相同的A在乘不同的B的時候不用每次都從頭乘
            for (ListNode* b = other.head->next; b != other.head; b = b->next) {
                int newCoef = a->coef * b->coef;
                int newExp = a->exp + b->exp;
                if (newCoef == 0) {continue;}

                ListNode* BIG = searchPtr->next;
                ListNode* pBIG = searchPtr;

                while(BIG != result.head && BIG->exp > newExp) {
                    pBIG = BIG;
                    BIG = BIG->next;
                }
                if(BIG != result.head && BIG->exp == newExp) {
                    BIG->coef+=newCoef;
                    if(BIG->coef==0) {
                        pBIG->next = BIG->next;
                        delete BIG;
                        searchPtr = pBIG;
                    }
                    else {
                        // 沒抵銷，指標停在當前點
                        searchPtr = BIG;
                    }
                }
                else {
                    ListNode* newNode = new ListNode(newCoef, newExp);
                    newNode->next = BIG;//circular
                    pBIG->next = newNode;
                    searchPtr = newNode;
                }
            }
        }
        return result;
    }
};

int main() {
    random_device rd;
    mt19937 rng(rd());

    int m = 100; // 固定 m
    vector<int> n_list = {10, 50, 100, 200, 300, 400, 600, 800, 1000, 1200, 2000, 3000}; 
    
    int num_runs = 10; 

    cout << "=== 實驗 1: Dense (稠密) 多項式 (平均 " << num_runs << " 次) ===" << endl;
    cout << "m \t n \t Avg_Time(microseconds)" << endl;

    for (int n : n_list) {
        double total_time = 0.0f;
        Polynomial A, B;
        A.generateDense(m);
        B.generateDense(n);
        for (int k = 0; k < num_runs; k++) {
            
            auto start = chrono::high_resolution_clock::now();

            Polynomial C = A.multiply(B);
            
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

            total_time += duration.count();
        }

        cout << m << " \t " << n << " \t " << (total_time / num_runs) << endl;
    }


    cout << "=== 實驗 2: Non Dense (稀疏) 多項式 (平均 " << num_runs << " 次) ===" << endl;
    cout << "m \t n \t Avg_Time(microseconds)" << endl;

    for (int n : n_list) {
        long long total_time = 0;
        for (int k = 0; k < num_runs; k++) {
            Polynomial A, B;
            A.generateSparse(m, rng);
            B.generateSparse(n, rng);

            auto start = chrono::high_resolution_clock::now();

            Polynomial C = A.multiply(B);

            auto end = chrono::high_resolution_clock::now();    
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

            total_time += duration.count();
        }
        
        cout << m << " \t " << n << " \t " << (total_time/num_runs) << endl;
    }

    return 0;
}