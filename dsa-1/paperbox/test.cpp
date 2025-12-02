#include <iostream>
using namespace std;

// 節點結構維持 struct (僅供 Class 內部使用)
struct ListNode {
    int coef;
    int exp;
    ListNode *next;
    ListNode(int c, int e) : coef(c), exp(e), next(nullptr) {}
};

class Polynomial {
private:
    ListNode* head; // 頭節點 (Dummy Head)

public:
    // 建構子：初始化 Dummy Head
    Polynomial() {
        head = new ListNode(0, 0);
        head->next = head; // 環狀串列
    }

    // 解構子：自動清理記憶體 (重要！不用在 main 手動 delete)
    ~Polynomial() {
        ListNode* current = head->next;
        while (current != head) {
            ListNode* temp = current;
            current = current->next;
            delete temp;
        }
        delete head;
    }

    // 功能 1: 新增一項 (封裝了指標操作，main 不用管 next)
    void addTerm(int coef, int exp) {
        // 這裡為了簡化，假設輸入時依照降冪輸入，直接插在 head 後面會變成升冪
        // 但你的作業輸入順序似乎是隨意的？
        // 為了相容你原本的寫法 (插在 head 後面)，我們照舊：
        ListNode* newNode = new ListNode(coef, exp);
        newNode->next = head->next;
        head->next = newNode;
        // 注意：如果需要嚴格排序插入，邏輯會不同。
        // 但你原本的 code 是直接插在 head 後面，這裡保持一致。
    }

    // 功能 2: 讓使用者輸入 (把 cin 邏輯藏起來)
    void input(string name) {
        int terms;
        cout << "cin " << name << " counts: ";
        cin >> terms;
        
        // 為了讓插入順序跟原本一樣 (保持降冪)，我們需要指針往後跑
        // 因為 addTerm 是插在頭，會反序。
        // 這裡重現你原本 main 裡的輸入邏輯：
        ListNode* temp = head;
        for (int i = 0; i < terms; i++) {
            int c, e;
            cout << "cin coef: "; cin >> c;
            cout << "cin exp: "; cin >> e;
            
            ListNode* newNode = new ListNode(c, e);
            temp->next = newNode; // 接在目前最後面
            newNode->next = head; // 圍成圈
            temp = newNode;       // 更新最後指針
        }
    }

    // 功能 3: 漂亮的輸出 (邏輯封裝)
    void print() const {
        ListNode* temp = head->next;
        bool isFirst = true;
        bool isEmpty = true;

        while (temp != head) {
            isEmpty = false;
            int c = temp->coef;
            int e = temp->exp;

            if (isFirst) {
                if (c < 0) cout << "-";
            } else {
                if (c >= 0) cout << " + ";
                else cout << " - ";
            }

            int absCoef = (c > 0) ? c : -c;
            
            if (e == 0) {
                cout << absCoef;
            } else {
                if (absCoef != 1) cout << absCoef;
                cout << "x";
                if (e != 1) cout << "^" << e;
            }

            isFirst = false;
            temp = temp->next;
        }
        if (isEmpty) cout << "0";
        cout << endl;
    }

    // 取得 head (給 multiply 用)
    ListNode* getHead() const { return head; }

    // 功能 4: 相乘 (核心邏輯，O(mn) 優化版)
    // 這裡回傳指標 Polynomial* 以避免淺層複製問題 (最簡單的解法)
    Polynomial* multiply(const Polynomial& other) {
        Polynomial* result = new Polynomial(); // 建立結果物件
        ListNode* poly_Ans = result->getHead();
        
        ListNode* poly1_curr = this->head->next;
        ListNode* poly2_head = other.getHead();

        while (poly1_curr != this->head) {
            ListNode* poly2_curr = poly2_head->next;
            ListNode* searchPtr = poly_Ans; // 優化關鍵

            while (poly2_curr != poly2_head) {
                int newCoef = poly1_curr->coef * poly2_curr->coef;
                int newExp = poly1_curr->exp + poly2_curr->exp;

                if (newCoef != 0) {
                    ListNode* multi = new ListNode(newCoef, newExp);
                    
                    ListNode* pBIG = searchPtr;
                    ListNode* BIG = pBIG->next;

                    while (BIG != poly_Ans && BIG->exp > multi->exp) {
                        pBIG = BIG;
                        BIG = BIG->next;
                    }

                    if (BIG != poly_Ans && BIG->exp == multi->exp) {
                        BIG->coef += multi->coef;
                        if (BIG->coef == 0) {
                            pBIG->next = BIG->next;
                            delete BIG;
                            searchPtr = pBIG;
                        } else {
                            searchPtr = BIG; // 修正後的優化邏輯
                        }
                        delete multi;
                    } else {
                        pBIG->next = multi;
                        multi->next = BIG;
                        searchPtr = multi; // 修正後的優化邏輯
                    }
                }
                poly2_curr = poly2_curr->next;
            }
            poly1_curr = poly1_curr->next;
        }
        return result;
    }
};

int main() {
    // 1. 建立物件
    Polynomial A, B;

    // 2. 輸入資料 (變得超乾淨！)
    A.input("term1");
    B.input("term2");

    // 3. 執行運算
    // 因為 multiply 回傳指標，我們用指標接
    Polynomial* C = A.multiply(B);

    // 4. 輸出結果
    cout << "Result: ";
    C->print();

    // 5. 記憶體清理
    delete C; // C 的解構子會自動清空它裡面的所有節點
    // A 和 B 在 main 結束時會自動呼叫解構子清理

    return 0;
}