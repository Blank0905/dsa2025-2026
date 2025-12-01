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

// 將多項式操作封裝成類別，方便管理和重複測試
class Polynomial {
private:
    ListNode* head; // Circular linked list 的頭節點 (Dummy head)

public:
    Polynomial() {
        head = new ListNode(0, 0); // Dummy node
        head->next = head; // Circular
    }

    // 解構子：清除記憶體 (重要！否則實驗跑多次會記憶體不足)
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

    // 取得 head (為了相容你的寫法)
    ListNode* getHead() const { return head; }

    // 加入一項 (用於生成測試資料)
    // 這裡我們簡單做，假設生成時已經排好序，直接加在 head->next (因為是降冪，新生成的較小指數往後插會比較慢，這裡為了生成速度簡化)
    // 更好的生成方式是直接串接，但為了模擬亂數插入，我們使用你的 insert 邏輯的簡化版
    void addTerm(int coef, int exp) {
        ListNode* newNode = new ListNode(coef, exp);
        // 直接插在 dummy head 後面 (測試資料生成順序若為升冪，這樣插就會變成降冪)
        newNode->next = head->next;
        head->next = newNode;
    }
    
    // 用於生成 Dense 測試資料 (指數連續)
    void generateDense(int terms) {
        clear();
        // 生成 x^0, x^1, ..., x^(terms-1)
        // 依序插入 head 後面，鏈結串列會變成 x^(terms-1) -> ... -> x^0 (降冪)
        for (int i = 0; i < terms; i++) {
            addTerm(1, i); // 係數設為 1
        }
    }

    // 用於生成 Non-dense 測試資料 (指數隨機分散)
    void generateSparse(int terms) {
        clear();
        vector<int> exps;
        int currentExp = 0;
        // 隨機間隔產生指數
        for (int i = 0; i < terms; i++) {
            currentExp += (rand() % 1000) + 1; // 間隔 1~100
            exps.push_back(currentExp);
        }
        // 因為 addTerm 是插在 head 後面，所以我們由小到大把指數插進去，
        // List 就會變成由大到小 (降冪)
        for (int e : exps) {
            addTerm(1, e);
        }
    }

    // 你的乘法邏輯 (封裝在內部)
    // 回傳一個新的 Polynomial 物件
    Polynomial multiply(const Polynomial& other) {
        Polynomial result;
        ListNode* poly_end = result.getHead(); // 你的 poly_end 其實就是 result 的 dummy head
        
        ListNode* poly1_head = this->head;
        ListNode* poly2_head = other.head;

        ListNode* temp1 = poly1_head->next;
        
        // 你的核心邏輯開始 -------------------------------------------
        while (temp1 != poly1_head) {
            ListNode* temp2 = poly2_head->next;
            ListNode* searchPtr = poly_end;
            while (temp2 != poly2_head) {
                
                // 計算係數與指數
                int newCoef = temp1->coef * temp2->coef;
                int newExp = temp1->exp + temp2->exp;

                if (newCoef != 0) { // 係數不為0才處理
                    ListNode* multi = new ListNode(newCoef, newExp);

                    // 尋找插入位置
                    ListNode* pBIG = searchPtr;
                    ListNode* BIG = searchPtr->next;

                    // [優化提示]：這裡每次都從頭找，是導致 O(m^2n) 的主因
                    while (BIG != poly_end && BIG->exp > multi->exp) {
                        pBIG = BIG;
                        BIG = BIG->next;
                    }

                    if (BIG != poly_end && BIG->exp == multi->exp) { // 指數相等，合併
                        BIG->coef += multi->coef;
                        if (BIG->coef == 0) {
                            pBIG->next = BIG->next;
                            delete BIG;
                            // 注意：這時 BIG 已經被刪除了，下次迴圈使用 BIG 可能會有問題？
                            // 幸好你的邏輯是繼續跑 temp2，不會再用到 BIG，但在這裡 BIG 變成懸空指標
                            searchPtr = pBIG; // 修正指標以防萬一
                        } else {
                            searchPtr = BIG;
                        }
                        delete multi;
                    } else { // 沒找到，插入
                        pBIG->next = multi;
                        multi->next = BIG;
                        searchPtr = multi;
                    }
                }
                temp2 = temp2->next;
            }
            temp1 = temp1->next;
        }
        // 你的核心邏輯結束 -------------------------------------------
        
        return result;
    }
};

int main() {
    srand(time(0)); // 設定隨機種子

    int m = 100; // 固定 m
    vector<int> n_list = {10, 100, 500, 1000, 2000}; // 測試不同的 n

    cout << "=== 實驗 1: Dense (稠密) 多項式 ===" << endl;
    cout << "m \t n \t Time(microseconds)" << endl;

    for (int n : n_list) {
        Polynomial A, B;
        A.generateDense(m);
        B.generateDense(n);

        // 開始計時
        auto start = chrono::high_resolution_clock::now();

        Polynomial C = A.multiply(B);

        // 結束計時
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

        cout << m << " \t " << n << " \t " << duration.count() << endl;
    }

    cout << "\n=== 實驗 2: Non-dense (稀疏) 多項式 ===" << endl;
    cout << "m \t n \t Time(microseconds)" << endl;

    for (int n : n_list) {
        Polynomial A, B;
        A.generateSparse(m);
        B.generateSparse(n);

        auto start = chrono::high_resolution_clock::now();

        Polynomial C = A.multiply(B);

        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

        cout << m << " \t " << n << " \t " << duration.count() << endl;
    }

    return 0;
}