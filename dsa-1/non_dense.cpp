#include <iostream>
#include <vector>
#include <chrono> // 用於計算時間
#include <random> // 用於亂數產生
#include <iomanip> // 用於輸出格式

using namespace std;

// 定義節點結構
struct Node {
    float coef;
    int exp;
    Node* next;
};

// 多項式類別
class Polynomial {
private:
    Node* head; // Head Node (首節點，不存資料)

public:
    // 建構子：建立一個只有 Head Node 的環狀串列
    Polynomial() {
        head = new Node;
        head->coef = 0;
        head->exp = -1;
        head->next = head; // 指向自己，形成環狀
    }

    // 解構子：釋放記憶體
    ~Polynomial() {
        Node* current = head->next;
        while (current != head) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
        delete head;
    }

    // 插入項 (保持指數由大到小排序)
    void insertTerm(float coef, int exp) {
        if (coef == 0) return;

        Node* prev = head;
        Node* curr = head->next;

        // 尋找插入位置
        while (curr != head && curr->exp > exp) {
            prev = curr;
            curr = curr->next;
        }

        // 如果指數相同，相加係數
        if (curr != head && curr->exp == exp) {
            curr->coef += coef;
            if (curr->coef == 0) { // 如果係數抵銷為0，刪除該節點
                prev->next = curr->next;
                delete curr;
            }
        } else {
            // 插入新節點
            Node* newNode = new Node;
            newNode->coef = coef;
            newNode->exp = exp;
            newNode->next = curr;
            prev->next = newNode;
        }
    }


    // 產生 Sparse 多項式 (指數隨機且分散)
    void generateSparse(int n) {
        clear();
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, n * 100); // 指數範圍很大，模擬稀疏

        for (int i = 0; i < n; i++) {
            insertTerm(1.0f, dis(gen));
        }
    }

    // 清空多項式 (保留 Head)
    void clear() {
        Node* current = head->next;
        while (current != head) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
        head->next = head;
    }

    // 顯示多項式 (除錯用)
    void print() {
        Node* curr = head->next;
        if (curr == head) {
            cout << "0" << endl;
            return;
        }
        while (curr != head) {
            cout << curr->coef << "x^" << curr->exp;
            if (curr->next != head) cout << " + ";
            curr = curr->next;
        }
        cout << endl;
    }

    // 靜態函式：多項式相乘 C = A * B
    // 這裡實作核心演算法
    static Polynomial multiply(Polynomial& A, Polynomial& B) {
        Polynomial C;
        Node* aPtr = A.head->next;

        // 遍歷 A 的每一項
        while (aPtr != A.head) {
            Node* bPtr = B.head->next;
            // 遍歷 B 的每一項
            while (bPtr != B.head) {
                float newCoef = aPtr->coef * bPtr->coef;
                int newExp = aPtr->exp + bPtr->exp;
                
                // 將結果插入 C (insertTerm 函式會處理排序和同類項合併)
                C.insertTerm(newCoef, newExp);
                
                bPtr = bPtr->next;
            }
            aPtr = aPtr->next;
        }
        return C;
    }
};

int main() {
    // 實驗設定 (參照圖片2的需求)
    int fixed_m = 100; // 固定 m
    vector<int> n_values = {10, 50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200}; // 調整 n (若電腦夠快可增加數值)

    cout << "=== Polynomial Multiplication Efficiency Test ===" << endl;
    cout << "Fixed m = " << fixed_m << endl << endl;

    // --- 測試: Sparse Case (稀疏/非稠密) ---
    cout << "--- Type: Sparse (Non-Dense) Polynomials ---" << endl;
    cout << setw(10) << "n" << setw(15) << "Time(ms)" << endl;
    cout << "-------------------------" << endl;

    for (int n : n_values) {
        Polynomial A, B;
        A.generateSparse(n);
        B.generateSparse(fixed_m);

        // 計時開始
        auto start = chrono::high_resolution_clock::now();
        
        Polynomial C = Polynomial::multiply(A, B);
        
        // 計時結束
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> duration = end - start;

        cout << setw(10) << n << setw(15) << duration.count() << endl;
    }

    cout << endl << "Experiment Completed. Copy the data above to Excel to plot the charts." << endl;

    return 0;
}