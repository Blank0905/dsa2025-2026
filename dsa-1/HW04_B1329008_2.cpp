#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <ctime>
#include <cstdlib>

using namespace std;

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

class Polynomial {
private:
    ListNode* head; //頭節點
public:
    Polynomial() {
        head = new ListNode(0, -1);
        head->next = head; // Circular
    }

    ~Polynomial() {
        ListNode* t = head->next;
        while (t != head) {
            ListNode* t2 = t->next;
            delete t;
            t = t2;
        }
        delete head;
    }

    void addTerm(int coef, int exp) {
        ListNode* newNode = new ListNode(coef, exp);
        newNode->next = head->next;
        head->next = newNode;
    }
    
    void generateDense(int terms) {// 生成 Dense
        for (int i = 0; i < terms; i++) {
            addTerm(1, i); 
        }
    }

    void generateSparse(int terms) {//生成non dense
        int currentExp = 0;
        for (int i = 1; i <= terms; ++i) {
            currentExp += i* (rand()% 100000) + 20; // 一定遞增
            addTerm(1, currentExp);
        }
    }

    void printPoly() {
        ListNode* curr = head->next; // 跳過head
        while (curr != head) {
            cout << curr->coef << "x^" << curr->exp;
            if (curr->next != nullptr) {cout << " + ";}
            curr = curr->next;
        }
        cout << endl;
    }

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
                        searchPtr = BIG;// 沒抵銷，指標停在當前點
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
    srand(time(0));
    int m = 100; // 固定 m
    
    vector<int> n_list = {10, 50, 100, 200, 300, 400, 500, 600, 800, 1000, 1200, 1500, 1800, 2000}; 
    
    int num_runs = 10; 
    Polynomial A, B;

   

    cout << "實驗 1: Dense (稠密) 多項式 (平均 " << num_runs << " 次)" << endl;
    cout << "m \t n \t Avg_Time(microseconds)" << endl;

    for (int n : n_list) {
        double total_time = 0;
        for (int k = 0; k < num_runs; k++) {
            Polynomial A, B;
            A.generateDense(m);
            B.generateDense(n);
            
            auto start = chrono::high_resolution_clock::now();

            Polynomial C = A.multiply(B);
            
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

            total_time += duration.count();
        }
        cout << m << " \t " << n << " \t " << (total_time / num_runs) << endl;
    }

    //B.generateSparse(10);
    //B.printPoly();


    cout << "實驗 2: Non Dense (稀疏) 多項式 (平均 " << num_runs << " 次)" << endl;
    cout << "m \t n \t Avg_Time(microseconds)" << endl;

    for (int n : n_list) {
        double total_time = 0;
        for (int k = 0; k < num_runs; k++) {
            Polynomial A, B;
            A.generateSparse(m);
            B.generateSparse(n);

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