#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>

using namespace std;

struct Node {
    float coef;
    int exp;
    Node* next;
};

class Polynomial {
private:
    Node* head;

public:
    Polynomial() {
        head = new Node;
        head->coef = 0;
        head->exp = -1;
        head->next = head;
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

    // 通用的插入函式 (會有搜尋時間，適用於 Sparse 或建立初始資料)
    void insertTerm(int coef, int exp) {
        if (coef == 0) return;
        Node* prev = head;
        Node* curr = head->next;
        while (curr != head && curr->exp > exp) {
            prev = curr;
            curr = curr->next;
        }
        if (curr != head && curr->exp == exp) {
            curr->coef += coef;
            if (curr->coef == 0) {
                prev->next = curr->next;
                delete curr;
            }
        } else {
            Node* newNode = new Node;
            newNode->coef = coef;
            newNode->exp = exp;
            newNode->next = curr;
            prev->next = newNode;
        }
    }

    // 快速插入 (用於從 Array 轉回 List)
    void appendTerm(float coef, int exp) {
        //if (coef == 0) return;
        Node* newNode = new Node;
        newNode->coef = coef;
        newNode->exp = exp;
        newNode->next = head->next;
        head->next = newNode;
    }
    
    // 取得最高次數 (用於決定 Array 大小)
    int getDegree() {
        if (head->next == head) return 0;
        return head->next->exp;
    }

    void generateDense(int n) {
        clear();
        for (int i = 0; i < n; i++) {
            insertTerm(1, i); 
        }
    }

    // 時間複雜度：O(mn)
    static Polynomial multiplyDenseOptimized(Polynomial& A, Polynomial& B) {
        Polynomial C;
        
        // 1. 計算結果的最大指數
        int maxDegA = A.getDegree();
        int maxDegB = B.getDegree();
        int maxDegC = maxDegA + maxDegB;

        // 2. 建立暫存陣列 (Bucket)，大小為 maxDegC + 1
        // 因為是 Dense，這裡不會浪費太多空間
        vector<float> bucket(maxDegC + 1, 0.0f);

        // 3. 執行乘法，直接存入陣列 (O(1) 存取)
        Node* aPtr = A.head->next;
        while (aPtr != A.head) {
            Node* bPtr = B.head->next;
            while (bPtr != B.head) {
                // 核心差異：不需要搜尋 Linked List，直接算 index
                bucket[aPtr->exp + bPtr->exp] += aPtr->coef * bPtr->coef;
                bPtr = bPtr->next;
            }
            aPtr = aPtr->next;
        }

        // 4. 將陣列轉回 Linked List
        // 為了保持 List 指數由大到小，我們從陣列 index 0 (最小) 開始遍歷，
        // 使用「頭插法」塞入 List，這樣最大的指數最後會排在最前面。
        for (int i = 0; i <= maxDegC; i++) {
            if (bucket[i] != 0.0f) {
                C.appendTerm(bucket[i], i);
            }
        }

        return C;
    }
};

int main() {
    // 改動 1: 把 m 加大，讓計算量變大，避免時間太短測不到
    int fixed_m = 100; 
    
    // 改動 2: n 的範圍調整
    vector<int> n_values = {50, 100, 200, 300, 400, 500, 800, 1000}; 

    cout << "=== Polynomial Multiplication Efficiency Test (Averaged) ===" << endl;
    cout << "Fixed m = " << fixed_m << endl << endl;

    cout << "--- Type: Dense (Optimized O(mn)) ---" << endl;
    cout << setw(10) << "n" << setw(20) << "Avg Time(ms)" << endl; // 顯示平均時間
    cout << "-----------------------------------" << endl;

    // 設定重複次數，用來消除誤差
    int repeat_count = 100; 

    for (int n : n_values) {
        Polynomial A, B;
        A.generateDense(fixed_m);
        B.generateDense(n);

        // 預熱 (Warm up)：先跑一次讓 CPU 喚醒，不算時間
        Polynomial temp = Polynomial::multiplyDenseOptimized(A, B);

        auto start = chrono::high_resolution_clock::now();
        
        // 改動 3: 每個 n 跑 100 次
        for(int k=0; k < repeat_count; k++) {
            Polynomial C = Polynomial::multiplyDenseOptimized(A, B);
        }
        
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> duration = end - start;

        // 算出平均單次時間
        double avg_time = duration.count() / repeat_count;

        cout << setw(10) << n << setw(20) << avg_time << endl;
    }

    return 0;
}