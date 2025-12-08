#include <iostream>
#include <vector>
#include <chrono>   // 用於計算時間
#include <random>   // 用於隨機生成
#include <algorithm> // 用於 sort

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
        head = new ListNode(0, 0); // Dummy node
        head->next = head; // Circular
    }

    // 解構子：清除記憶體
    ~Polynomial() {
        clear();
        delete head;
    }

    void clear() {
        ListNode* current = head->next;
        while (current != head) {
            ListNode* temp = current;
            current = current->next;
            delete temp;
        }
        head->next = head;
    }

    // 取得 head
    ListNode* getHead() const { return head; }

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

    

    // 生成 Sparse 測試資料 (指數隨機分散)
    void generateSparse(int terms) {
        clear();
        vector<int> exps;
        int currentExp = 0;
        for (int i = 0; i < terms; i++) {
            currentExp += (rand() % 100000); // 間隔 100~1000
            exps.push_back(currentExp);
        }
        // 由於 addTerm 是插頭，這裡依序插入會變成降冪排列
        for (int e : exps) {
            addTerm(1, e);
        }
    }

    // 核心乘法邏輯
    Polynomial multiply(const Polynomial& other) {
        Polynomial result;
        ListNode* poly_end = result.getHead(); // result 的 dummy head
        
        ListNode* poly1_head = this->head;
        ListNode* poly2_head = other.head;

        ListNode* temp1 = poly1_head->next;
        
        // 外層迴圈：遍歷 A
        while (temp1 != poly1_head) {
            ListNode* temp2 = poly2_head->next;
            
            // searchPtr 在這裡重置 (針對每一個 A 的項次，重頭開始找 Result 的插入點)
            // 但在內層迴圈跑 B 的時候，因為降冪特性，不需要回頭
            ListNode* searchPtr = poly_end; 

            // 內層迴圈：遍歷 B
            while (temp2 != poly2_head) {
                
                int newCoef = temp1->coef * temp2->coef;
                int newExp = temp1->exp + temp2->exp;

                if (newCoef != 0) {
                    // 【優化重點】：這裡先不 new ListNode，避免合併時浪費記憶體操作
                    
                    ListNode* pBIG = searchPtr;
                    ListNode* BIG = searchPtr->next;

                    // 尋找插入位置 (利用整數比較，速度快)
                    // 由於 searchPtr 記住了上次的位置，這裡通常只需要比較很少次
                    while (BIG != poly_end && BIG->exp > newExp) {
                        pBIG = BIG;
                        BIG = BIG->next;
                    }

                    if (BIG != poly_end && BIG->exp == newExp) {
                        // --- 情境 A: 指數相同 -> 合併 ---
                        // 完全不需要 new 節點，直接改數值
                        BIG->coef += newCoef;
                        
                        if (BIG->coef == 0) {
                            // 係數抵銷為 0，刪除節點
                            pBIG->next = BIG->next;
                            delete BIG;
                            // 刪除後，指標退回前一個安全點
                            searchPtr = pBIG; 
                        } else {
                            // 沒抵銷，指標停在當前點
                            searchPtr = BIG;
                        }
                    } else {
                        // --- 情境 B: 指數不同 -> 插入 ---
                        // 確定要插入了，才執行 new (Lazy Allocation)
                        ListNode* multi = new ListNode(newCoef, newExp);
                        pBIG->next = multi;
                        multi->next = BIG;
                        
                        // 更新指標到新插入的點，下次從這繼續往後
                        searchPtr = multi;
                    }
                }
                temp2 = temp2->next;
            }
            temp1 = temp1->next;
        }
        
        return result;
    }

    Polynomial multiplyNaive(const Polynomial& other) const {
        Polynomial result;
        ListNode* resHead = result.getHead();
    
        for (ListNode* a = head->next; a != head; a = a->next) {
            for (ListNode* b = other.head->next; b != other.head; b = b->next) {
                int newCoef = a->coef * b->coef;
                int newExp = a->exp + b->exp;
                if (newCoef == 0) continue;
    
                // 從頭開始掃（每一對 (a,b) 都從 dummy head 開始）
                ListNode* p = resHead;
                ListNode* cur = p->next;
                while (cur != resHead && cur->exp > newExp) {
                    p = cur;
                    cur = cur->next;
                }
    
                if (cur != resHead && cur->exp == newExp) {
                    cur->coef += newCoef;
                    if (cur->coef == 0) {
                        p->next = cur->next;
                        delete cur;
                    }
                } else {
                    ListNode* node = new ListNode(newCoef, newExp);
                    p->next = node;
                    node->next = cur;
                }
            }
        }
        return result;
    }
};

int main() {
    srand(time(0)); 

    int m = 100; // 固定 m
    vector<int> n_list = {10, 50, 100, 200, 300, 400, 600, 800, 1000, 1200, 2000, 3000}; 
    
    int num_runs = 5; 

    cout << "=== 實驗 1: Dense (稠密) 多項式 (平均 " << num_runs << " 次) ===" << endl;
    cout << "m \t n \t Avg_Time(microseconds)" << endl;

    for (int n : n_list) {
        long long total_time = 0;

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


    cout << "=== 實驗 2: Non Dense (稀疏) 多項式 (平均 " << num_runs << " 次) ===" << endl;
    cout << "m \t n \t Avg_Time(microseconds)" << endl;

    for (int n : n_list) {
        long long total_time = 0;
        for (int k = 0; k < num_runs; k++) {
            Polynomial A, B;
            A.generateSparse(m);
            B.generateSparse(n);

            auto start = chrono::high_resolution_clock::now();

            Polynomial C = A.multiplyNaive(B);

            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
                
            total_time += duration.count();
        }
        
        cout << m << " \t " << n << " \t " << (total_time/num_runs) << endl;
    }

    return 0;
}