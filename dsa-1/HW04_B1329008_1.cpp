#include <iostream>

using namespace std;

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

ListNode* multiply (ListNode* poly1, ListNode* poly2) {
    ListNode* temp = new ListNode (poly1 -> coef * poly2 -> coef, poly1 -> exp + poly2 -> exp);
    if (temp -> coef == 0) {//防輸入係數0
        delete temp;
        return nullptr;
    }
    else {
        return temp;
    }
}

int main () {
    int term1 = 0;
    int term2 = 0;
    ListNode* temp1;
    ListNode* poly1_head = new ListNode(0, 0);
    poly1_head -> next = poly1_head;
    ListNode* poly2_head = new ListNode(0, 0);
    poly2_head -> next = poly2_head;
    ListNode* poly_Ans = new ListNode (0, 0);//最後答案的頭node
    poly_Ans -> next = poly_Ans;

    cout << "cin term1\n";
    cin >> term1;
    temp1 = poly1_head;
    for (int i = 0; i < term1; i++) {
        int coef = 0;
        int exp = 0;
        cout << "cin coef\n";
        cin >> coef;
        cout << "cin exp\n";
        cin >> exp;
        ListNode* poly1 = new ListNode(coef, exp);
        temp1 -> next = poly1;
        poly1 -> next = poly1_head;
        temp1 = poly1;
    }
    cout << "cin term2\n";
    cin >> term2;
    temp1 = poly2_head;
    for (int i = 0; i < term2; i++) {
        int coef = 0;
        int exp = 0;
        cout << "cin coef\n";
        cin >> coef;
        cout << "cin exp\n";
        cin >> exp;
        ListNode* poly2 = new ListNode(coef, exp);
        temp1 -> next = poly2;
        poly2 -> next = poly2_head;
        temp1 = poly2;
    }


    
    temp1 = poly1_head -> next;
    while (temp1 != poly1_head) {
        ListNode* temp2 = poly2_head -> next;
        ListNode* searchPtr = poly_Ans;
        while (temp2 != poly2_head) {
            int newCoef = temp1->coef * temp2->coef;
            int newExp = temp1->exp + temp2->exp;
            
            //ListNode* multi = multiply(temp1, temp2);

            if (newCoef == 0) {
                temp2 = temp2 -> next;
                continue;
            }
            ListNode* pBIG = searchPtr;
            ListNode* BIG = pBIG -> next;
            while (BIG != poly_Ans && BIG -> exp > newExp) {//還沒找到的時候一直往後找，找到最後沒找到就往下，後面會直接接到最後面
                pBIG = BIG;
                BIG = BIG -> next;
            }
            if (BIG != poly_Ans && BIG -> exp == newExp) { //找到且已經有相同指數的項
                BIG -> coef = BIG -> coef + newCoef;
                if (BIG -> coef == 0) {
                    pBIG -> next = BIG -> next;
                    delete BIG;
                    searchPtr = pBIG;
                }
                else {
                    searchPtr = BIG;
                }
            }
            else {//找到他比目前BIG指標的指數還大，或是已經到最後了，都直接接到BIG的前面/這邊multi備插到答案裡了不用刪掉
                ListNode* multi = new ListNode(newCoef, newExp);
                pBIG -> next = multi;
                multi -> next = BIG;
                searchPtr = multi;
            }
            temp2 = temp2 -> next;
        }
        temp1 = temp1 -> next;
    }

    cout << "Result: ";//列印結果
    temp1 = poly_Ans -> next;
    bool isFirst = true; // 用來判斷是不是第一項 (第一項的正號不用印)
    
    while (temp1 != poly_Ans) {
        int c = temp1->coef;
        int e = temp1->exp;
        
        // 1. 處理正負號與連接符
        if (isFirst) {
            // 第一項：如果是負的印負號，正的不用印符號
            if (c < 0) cout << "-";
        } else {
            // 非第一項：正數印 " + "，負數印 " - "
            if (c >= 0) cout << " + "; 
            else cout << " - ";       
        }

        // 2. 取得係數絕對值 (因為符號在上面印過了)
        int absCoef = (c > 0) ? c : -c;

        // 3. 處理係數與變數顯示
        if (e == 0) {
            // Case A: 指數為 0 (常數項)，例如 5x^0 -> 5
            // 直接印數字，不印 x
            cout << absCoef; 
        } 
        else {
            // Case B: 指數不為 0
            
            // 如果係數不是 1，要印出來 (例如 3x)。
            // 如果係數是 1 (例如 1x)，通常省略 1，只印 x (除非你想印 1x)
            if (absCoef != 1) {
                cout << absCoef;
            }
            
            cout << "x";
            
            // 4. 處理指數顯示
            // 如果指數不是 1，才印 ^n (例如 x^2)
            // 如果指數是 1，不印 ^1 (例如 x^1 -> x)
            if (e != 1) {
                cout << "^" << e;
            }
        }

        isFirst = false;
        temp1 = temp1 -> next;
    }

    if (isFirst) cout << "0"; // 如果結果是空的 (0)，印出 0
    cout << endl;

    return 0;
}