#include <iostream>
using namespace std;

struct ListNode {
    int coef;
    int exp;
    ListNode* next;
    ListNode (int c, int e) {
        coef = c;
        exp = e;
        next = nullptr;
    }
};

class Polynomial {
private:
    ListNode* head;
public:
    Polynomial() {
        head = new ListNode(0, 0);
        head->next = head;
    }

    ~Polynomial() {
        ListNode* curr = head->next;
        while(curr != head) {
            ListNode* temp = curr;
            curr = curr->next;
            delete temp;
        }
        delete head;
    }

    void addTerm(int coef, int exp) {//土立土及
        ListNode* newNode = new ListNode(coef, exp);
        
    }

    void input(string name) {//把多項式輸入進去
        int terms;
        cout << "輸入" << name << "有幾項:";
        ListNode* temp = head;
        cin >> terms;
        for(int i = 0; i<terms; i++) {
            int c;
            cout << "輸入係數:";
            cin >> c;
            int e;
            cout << "輸入幾次方:";
            cin >> e;

            ListNode* newNode = new ListNode(c, e);    
            newNode->next = head;
            temp -> next = newNode;
            temp = temp->next;
        }
        
    }
    
    ListNode* getHead() {
        return head;
    }

    Polynomial* multiply(Polynomial& poly) {
        Polynomial* result = new Polynomial();
        ListNode* poly_Ans = result->getHead();

        ListNode* poly1_curr = this->head->next;
        ListNode* poly2_head = poly.getHead();

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
};

int main() {
    Polynomial A, B;

    A.input("Polynomial A");
    B.input("Polynomial B");

    Polynomial* C = A.multiply(B);

    C->print();

    delete C;

    return 0;
}
