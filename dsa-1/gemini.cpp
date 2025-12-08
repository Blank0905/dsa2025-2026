#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>
#include <algorithm> // for sort

using namespace std;

struct Node {
    float coef;
    int exp;
    Node* next;
};

class Polynomial {
public: // 為了方便實驗，先設為 public，實際專案應為 private
    Node* head;

public:
    Polynomial() {
        head = new Node;
        head->coef = 0;
        head->exp = -1; // Dummy head
        head->next = head; // Circular linked list
    }

    ~Polynomial() {
        clear();
        delete head;
    }

    void clear() {
        Node* current = head->next;
        while (current != head) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
        head->next = head;
    }

    // 快速插入 (用於建立初始資料，已知由大到小)
    void appendTerm(float coef, int exp) {
        if (coef == 0) return;
        Node* newNode = new Node;
        newNode->coef = coef;
        newNode->exp = exp;
        newNode->next = head->next; // 頭插法
        head->next = newNode;
    }

    // ==========================================
    // 核心演算法：將另一個多項式加到自己身上 (Merge)
    // ==========================================
    void add(Polynomial& polyToAdd) {
        Node* p1 = head->next;       // 自己的游標
        Node* prev1 = head;          // p1 的前一個
        Node* p2 = polyToAdd.head->next; // 對方的游標

        while (p1 != head && p2 != polyToAdd.head) {
            if (p1->exp == p2->exp) {
                // 指數相同 -> 係數相加
                p1->coef += p2->coef;
                if (p1->coef == 0) { // 抵銷變0，刪除節點
                    prev1->next = p1->next;
                    delete p1;
                    p1 = prev1->next;
                } else {
                    prev1 = p1;
                    p1 = p1->next;
                }
                p2 = p2->next; 
            } 
            else if (p1->exp < p2->exp) { 
                // 我們是「由大到小」排序。
                // 如果 p2(對方) 的指數比 p1(自己) 大，代表 p2 應該插在 p1 前面
                Node* newNode = new Node;
                newNode->coef = p2->coef;
                newNode->exp = p2->exp;
                
                newNode->next = p1;
                prev1->next = newNode;
                
                prev1 = newNode; 
                // p1 不動，繼續跟下一個比較
                p2 = p2->next;
            } 
            else {
                // p1 指數比 p2 大，p1 自己往下走
                prev1 = p1;
                p1 = p1->next;
            }
        }

        // 如果 p2 還有剩，全部接在 p1 後面
        while (p2 != polyToAdd.head) {
            Node* newNode = new Node;
            newNode->coef = p2->coef;
            newNode->exp = p2->exp;
            
            newNode->next = p1; 
            prev1->next = newNode;
            prev1 = newNode;
            
            p2 = p2->next;
        }
    }

    // ==========================================
    // 實驗主角：通用乘法 (Row-wise Addition)
    // ==========================================
    static Polynomial multiplyRowWise(Polynomial& A, Polynomial& B) {
        Polynomial C;
        
        // 遍歷 A 的每一項
        Node* ptrA = A.head->next;
        while (ptrA != A.head) {
            
            // 建立一個暫存列 Temp = (A的一項) * (整個B)
            Polynomial Temp; 
            Node* ptrB = B.head->next;
            
            // 這裡注意：因為 B 是從大到小，乘出來的結果如果要保持從大到小，
            // 我們可以使用 appendTerm (頭插法) 但要從 B 的尾巴讀回來？
            // 為了簡化實驗，我們這裡直接依序乘，但因為 appendTerm 是頭插，
            // 我們如果順著 B 讀，Temp 會變成「從小到大」。
            // 所以這裡小技巧：我們先把結果存在 vector，再反向塞入 Temp，確保 Temp 是「由大到小」
            // 這樣才能餵給 add 函式。
            
            // 實際上，為了效能，我們可以寫一個 appendTail (尾插)，這裡為了不改動太多結構，
            // 我們假設 B 是由小到大遍歷 (因為我們生成資料時控制就好)，
            // 或者我們這裡做一個簡單的 vector buffer。
            
            // 修正策略：直接用 vector 存這一排，然後由小到大 appendTerm 變成由大到小
            vector<pair<float, int>> rowBuffer;
            while (ptrB != B.head) {
                rowBuffer.push_back({ptrA->coef * ptrB->coef, ptrA->exp + ptrB->exp});
                ptrB = ptrB->next;
            }
            
            // rowBuffer 目前是 [大, 中, 小]
            // appendTerm 是頭插，所以我們要從 [小] 開始插，才會變成 [大, 中, 小]
            for(int i = rowBuffer.size()-1; i >= 0; i--) {
                Temp.appendTerm(rowBuffer[i].first, rowBuffer[i].second);
            }

            // 將這一排加進結果 C
            C.add(Temp);

            ptrA = ptrA->next;
        }
        return C;
    }

    // 輔助：生成測試資料
    void generate(int n, bool isSparse) {
        clear();
        vector<int> exps;
        if (!isSparse) {
            // Dense: 0, 1, 2... n-1
            for(int i=0; i<n; i++) exps.push_back(i);
        } else {
            // Sparse: 隨機產生 n 個不重複的數字，範圍很大
            // 這裡簡單用間隔法模擬 Sparse，保證不重複且分散
            int current = 0;
            for(int i=0; i<n; i++) {
                current += (rand() % 100) + 10; // 每個指數間隔 10~100
                exps.push_back(current);
            }
        }
        
        // 因為 appendTerm 是頭插法，我們給由小到大的數據，List 就會變成由大到小
        // exps 目前是由小到大
        for (int e : exps) {
            appendTerm(1.0f, e); // 圖片要求 coef = 1
        }
    }
};

int main() {
    srand(time(0));
    int fixed_m = 100; // 圖片要求固定 m
    vector<int> n_values = {10, 50, 100, 200, 500, 1000, 1500, 2000}; 

    cout << "=== Polynomial Experiment: Row-wise Addition ===" << endl;
    cout << "Fixed m = " << fixed_m << ", Coef = 1" << endl;
    cout << setw(10) << "n" << setw(18) << "Dense(ms)" << setw(18) << "Sparse(ms)" << endl;
    cout << "------------------------------------------------" << endl;

    for (int n : n_values) {
        double avg_dense = 0, avg_sparse = 0;
        int trials = 10; // 取平均值減少誤差

        // 1. 測試 Dense
        for(int t=0; t<trials; t++) {
            Polynomial A, B;
            A.generate(fixed_m, false); // m (Dense)
            B.generate(n, false);       // n (Dense)

            auto start = chrono::high_resolution_clock::now();
            Polynomial C = Polynomial::multiplyRowWise(A, B);
            auto end = chrono::high_resolution_clock::now();
            avg_dense += chrono::duration<double, milli>(end - start).count();
        }

        // 2. 測試 Sparse
        for(int t=0; t<trials; t++) {
            Polynomial A, B;
            A.generate(fixed_m, true); // m (Sparse)
            B.generate(n, true);       // n (Sparse)

            auto start = chrono::high_resolution_clock::now();
            Polynomial C = Polynomial::multiplyRowWise(A, B);
            auto end = chrono::high_resolution_clock::now();
            avg_sparse += chrono::duration<double, milli>(end - start).count();
        }

        cout << setw(10) << n 
             << setw(18) << avg_dense / trials 
             << setw(18) << avg_sparse / trials << endl;
    }

    return 0;
}