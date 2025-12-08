#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono> // 用於計時
#include <random> // 用於隨機生成

using namespace std;

// --- 你的原始資料結構 ---
struct ListNode {
    int coef;
    int exp;
    ListNode *next;
    ListNode (int x, int y) {
        coef = x;
        exp = y;
        next = nullptr;
    }
};

// --- 你的原始相乘函式 ---
ListNode* multiply (ListNode* poly1, ListNode* poly2) {
    // 注意：這裡直接乘可能會溢出 int，但在測試複雜度時不影響時間，故不改動
    ListNode* temp = new ListNode (poly1 -> coef * poly2 -> coef, poly1 -> exp + poly2 -> exp);
    if (temp -> coef == 0) {
        delete temp;
        return nullptr;
    }
    else {
        return temp;
    }
}

// --- 輔助函式：釋放記憶體 (避免測試多次後記憶體不足) ---
void deleteList(ListNode* head) {
    if (!head) return;
    ListNode* current = head->next;
    while (current != head) {
        ListNode* temp = current;
        current = current->next;
        delete temp;
    }
    delete head;
}

// --- 輔助函式：生成多項式 ---
// isDense = true: 生成連續指數 (如 99, 98, ..., 0)
// isDense = false: 生成大範圍隨機跳躍指數 (如 50000, 1205, 30...) 以模擬極端插入情況
ListNode* generatePoly(int terms, bool isDense) {
    ListNode* head = new ListNode(0, 0);
    head->next = head;
    ListNode* tail = head;

    vector<int> exps;
    if (isDense) {
        // 稠密：指數從 terms-1 到 0
        for (int i = 0; i < terms; i++) {
            exps.push_back(i); 
        }
    } else {
        // 稀疏：指數在 0 到 100000 之間隨機選取，確保間隔大
        // 使用 static 隨機引擎確保每次呼叫都不同
        static random_device rd;
        static mt19937 gen(rd());
        uniform_int_distribution<> dis(0, 100000); // 極大範圍
        
        
        for (int i = 0; i < terms; i++) {
            int e = dis(gen);
            // 避免重複指數簡單處理 (雖然你的程式能處理合併，但為了控制項數，我們盡量不重複)
            exps.push_back(e);
        }
    }

    // 排序：通常輸入多項式是降冪排列
    sort(exps.rbegin(), exps.rend());

    // 建立 Linked List
    for (int e : exps) {
        ListNode* newNode = new ListNode(1, e); // 係數固定為 1，專注測指數邏輯
        tail->next = newNode;
        newNode->next = head;
        tail = newNode;
    }
    return head;
}

// --- 你的核心邏輯封裝 (用於測試) ---
// 回傳值：執行時間 (microseconds)
long long run_multiplication_logic(ListNode* poly1_head, ListNode* poly2_head) {
    
    // 計時開始
    auto start = chrono::high_resolution_clock::now();

    ListNode* poly_Ans = new ListNode (0, 0);
    poly_Ans -> next = poly_Ans;

    ListNode* temp1 = poly1_head -> next;
    ListNode* temp2;
    ListNode* temp3; // 你的代碼中有宣告但未明顯使用，保留

    while (temp1 != poly1_head) {
        temp2 = poly2_head -> next;
        // temp3 = poly_Ans; // 你的原始代碼
        ListNode* searchPtr = poly_Ans;
        
        while (temp2 != poly2_head) {
            ListNode* multi = multiply(temp1, temp2);
            if (multi == nullptr) {
                temp2 = temp2 -> next;
                continue;
            }
            
            // --- 你的插入邏輯 ---
            ListNode* pBIG = searchPtr;
            ListNode* BIG = pBIG -> next;
            
            // 尋找插入點
            while (BIG != poly_Ans && BIG -> exp > multi -> exp) {
                pBIG = BIG;
                BIG = BIG -> next;
            }

            if (BIG != poly_Ans && BIG -> exp == multi -> exp) { 
                // 指數相同，相加
                BIG -> coef = BIG -> coef + multi -> coef;
                if (BIG -> coef == 0) {
                    pBIG -> next = BIG -> next;
                    delete BIG;
                    // searchPtr = pBIG; // 你的邏輯
                    // 修正：刪除節點後，searchPtr 應該指向上一個有效節點，以便下次從這裡開始
                    searchPtr = pBIG; 
                }
                else {
                    searchPtr = BIG;
                }
                delete multi;
            }
            else { 
                // 插入新節點
                pBIG -> next = multi;
                multi -> next = BIG;
                searchPtr = multi;
            }
            temp2 = temp2 -> next;
        }
        temp1 = temp1 -> next;
    }

    // 計時結束
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    
    // 清理結果 List
    deleteList(poly_Ans);

    return duration.count();
}

int main () {
    // 設定測試參數
    int fixed_m = 100; // 固定 Term1 項數
    vector<int> n_values = {100, 200, 400, 600, 800, 1000, 1200}; // 變動 Term2 項數
    
    cout << "--- Experiment Start ---\n";
    cout << "Fixed m (Term1) = " << fixed_m << endl;
    cout << "Unit: Microseconds (us)\n\n";

    // 輸出表頭
    cout << "n(Term2)\tDense_Avg_Time(us)\tNon-Dense_Time(us)" << endl;

    for (int n : n_values) {
        
        // --- 1. Dense Test (跑 10 次取平均) ---
        long long dense_total_time = 0;
        for (int k = 0; k < 10; k++) {
            ListNode* p1 = generatePoly(fixed_m, true); // Dense
            ListNode* p2 = generatePoly(n, true);       // Dense
            dense_total_time += run_multiplication_logic(p1, p2);
            deleteList(p1);
            deleteList(p2);
        }
        long long dense_avg = dense_total_time / 10;

        // --- 2. Non-Dense (Sparse) Test ---
        // 生成極端稀疏矩陣：指數範圍大，係數為1
        // 這裡我們也跑 3 次取平均以防隨機誤差過大，或者依照你要求只跑一次也可
        long long sparse_total_time = 0;
        int sparse_runs = 3; 
        for(int k=0; k<sparse_runs; k++){
            ListNode* p1 = generatePoly(fixed_m, true); // Dense
            ListNode* p2 = generatePoly(n, false);       // Sparse
            sparse_total_time += run_multiplication_logic(p1, p2);
            deleteList(p1);
            deleteList(p2);
        }
        long long sparse_avg = sparse_total_time / sparse_runs;

        // 輸出結果 (Tab 分隔，方便貼到 Excel)
        cout << n << "\t\t" << dense_avg << "\t\t\t" << sparse_avg << endl;
    }

    cout << "\n--- Experiment End ---" << endl;
    return 0;
}