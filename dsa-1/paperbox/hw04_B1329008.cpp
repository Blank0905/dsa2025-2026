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
    ListNode* poly_end = new ListNode (0, 0);//最後答案的頭node
    poly_end -> next = poly_end;

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

    ListNode* temp2;
    ListNode* temp_end = poly_end;
    ListNode* temp3;
    

    temp1 = poly1_head -> next;
    while (temp1 != poly1_head) {
        temp2 = poly2_head -> next;
        temp3 = poly_end;
        while (temp2 != poly2_head) {
            ListNode* multi = multiply(temp1, temp2);
            if (multi == nullptr) {
                temp2 = temp2 -> next;
                continue;
            }
            ListNode* pBIG = poly_end;
            ListNode* BIG = poly_end -> next;
            while (BIG != poly_end && BIG -> exp > multi -> exp) {
                pBIG = BIG;
                BIG = BIG -> next;
            }
            if (BIG != poly_end && BIG -> exp == multi -> exp) { //找到且指數相等
                BIG -> coef = BIG -> coef + multi -> coef;
                if (BIG -> coef == 0) {
                    pBIG -> next = BIG -> next;
                    delete BIG;
                }
                delete multi;
            }
            else {
                pBIG -> next = multi;
                multi -> next = BIG;
            }
            temp2 = temp2 -> next;
        }
        temp1 = temp1 -> next;
    }

    //for testing
    temp1 = poly_end -> next;
    while (temp1 != poly_end) {
        if (temp1 -> next != poly_end) {
            cout << temp1 -> coef << "x^" << temp1 -> exp << "+";
        }
        else {
             cout << temp1 -> coef << "x^" << temp1 -> exp;
        }
        temp1 = temp1 -> next;
    }
    
    
    return 0;
}