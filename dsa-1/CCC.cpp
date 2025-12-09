#include<iostream>
#include<cstdlib>
#include<ctime>
#include<chrono>
#include<algorithm>
#include<vector>
#include<random>
#include<set>
using namespace std;

struct Node{
    int coef;
    int exp;
    Node *next;
    Node(int c=0,int e=0){
        coef = c;
        exp = e;
        next = nullptr;
    }
};

class poly{
public:
    Node *head;

    poly(){
        head = new Node();
        head->next = head;
    }

    ~poly () {
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



    poly multi(poly& poly1){
         poly result;
            for(Node *p=head->next;p!=head;p=p->next){
                Node* pos=result.head;
                for(Node *q=poly1.head->next;q!=poly1.head;q=q->next){
                    int c = p->coef*q->coef;
                    int e = p->exp+q->exp;
                    Node *pre = pos;
                    Node *current =pos->next;       
                    while(current!=result.head && current->exp>e){
                        pre=current;
                        current=current->next;
                    }
                    if(current !=result.head && current->exp==e){
                        current->coef+=c;
                        if(current->coef==0){
                            pre->next=current->next;
                            delete current;
                            pos=pre;
                        }
                        else{
                            pos=current;
                        }
                        continue;
                    }

                    Node *node =new Node(c,e);
                    pre->next=node;
                    node->next=current;
                    pos=node;
                }
            }
            return result;
    }
};



int randomInt(int l, int r){
    return l + rand() % (r - l + 1);
}


void generateNonDensePoly(poly &p, int size){
    std::set<int> exps;
    int range = size * 10;

    while(exps.size() < size){
        exps.insert(rand() % range);
    }

    Node *cur = p.head;
    for(auto it = exps.rbegin(); it != exps.rend(); ++it){
        Node *node = new Node(1, *it);
        cur->next = node;
        node->next = p.head;
        cur = node;
    }
}


void generateDensePoly(poly &p, int size){
    int maxExp = randomInt(size, size + 1000);
    Node *cur = p.head;

    for(int i = 0; i < size; i++){
        Node *node = new Node(1, maxExp - i);
        cur->next = node;
        node->next = p.head;
        cur = node;
    }
}



int main(){
    srand(time(NULL));

    int m = 100;      
    int repeat = 300; 

    
    const int tn = 2000 / 200;   
    int testN[tn];
    for(int i = 0; i < tn; i++){
        testN[i] = (i + 1) * 200;
    }

    cout << "m = " << m << endl;


    cout << "\nNon-Dense\n";
    cout << "n\tAvg Time(ms)\n";

    for(int k = 0; k < tn; k++){
        int n = testN[k];
        double total = 0;

        for(int r = 0; r < repeat; r++){
            poly a, b;
            generateNonDensePoly(a, m);
            generateNonDensePoly(b, n);

            auto s = chrono::high_resolution_clock::now();
            poly c = a.multi(b);
            auto e = chrono::high_resolution_clock::now();

            total += chrono::duration<double, milli>(e - s).count();
        }

        cout << n << "\t" << total / repeat << endl;
    }


    cout << "\nDense\n";
    cout << "n\tAvg Time(ms)\n";

    for(int k = 0; k < tn; k++){
        int n = testN[k];
        double total = 0;

        for(int r = 0; r < repeat; r++){
            poly a, b;
            generateDensePoly(a, m);
            generateDensePoly(b, n);

            auto s = chrono::high_resolution_clock::now();
            poly c = a.multi(b);
            auto e = chrono::high_resolution_clock::now();

            total += chrono::duration<double, milli>(e - s).count();
        }

        cout << n << "\t" << total / repeat << endl;
    }

    return 0;
}