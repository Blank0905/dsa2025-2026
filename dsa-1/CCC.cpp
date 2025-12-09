#include <iostream>
#include <stdexcept>
#include <string>
#include <cstring>
#include <algorithm>
#include <random>
#include <cstdlib>
#include <chrono>
using namespace std;

class node{
    public:
    int coef;
    int exp;
    node* next = nullptr;
    node(int c, int e){
        coef = c;
        exp = e;
    }
};

class poly{
    private:

    public:
    node* header = new node(0,-1);  
    node* last = header;
    poly(){
        header->next = header;
        
    }
    ~poly(){
        node* temp;
        for(node* n = header->next;n!=header;n = temp){
            temp = n->next;
            delete n;
        }
        delete header;
    }

    void generateRand(int terms,bool dense){
        vector<int> exps;
        if(dense){
            for(int i = terms-1;i>=0;i--){
                exps.push_back(i);
            }
        }
        else{
            for(int i = 0;i < terms;i++){
                int e;
                do{
                    e = rand() % terms*50;
                }while(find(exps.begin(),exps.end(), e) != exps.end());
                exps.push_back(e);
            }
        }
        for(int i : exps){
            add_term(1,i,nullptr);
        }
    }

    node* add_term(int coef,int exp,node* start){
        
        if(start == nullptr){
            node* n = new node(coef,exp);
            last->next = n;
            last = n;
            n->next = header;
            return nullptr;
        }

        node* prev = start;
        node* cur = start->next;
        while(cur != header && cur->exp > exp){
            prev = cur;
            cur = cur->next;                
        }
        if(cur!=header && cur->exp == exp){
            cur->coef+=coef;
            
        }
        else{
            node* n = new node(coef,exp);
            n->next = cur;
            cur = n;
            prev->next = n;
            
            if(cur == header){
                last = n;
            }
            return n;
        }
        return cur;

    }

};

int main(){
    // vector<int> x = {10,50,100,200,300,400,500,600,700,800,900,1000,1200};
    vector<int> x;
    for(int i = 0;i<=1000;i+=15){
        x.push_back(i);
    }
    int m = 100;
    bool danse = true;

    for(int n :x){
        double total_time = 0;
        for(int j = 0;j<1000;j++){
            clock_t start_time,end_time;
            double this_time;
            
            poly* a = new poly;
            poly* b = new poly;
            poly* c = new poly;
            a->generateRand(n,danse);
            b->generateRand(m,danse);
            
            start_time = clock();
            // auto start = chrono::high_resolution_clock::now();
            for(node* a_cur = a->header->next;a_cur!=a->header;a_cur = a_cur->next){

                node* c_cur = c->header;
                for(node* b_cur = b->header->next;b_cur!=b->header;b_cur = b_cur->next){
                    int coef = a_cur->coef*b_cur->coef;
                    int e = a_cur->exp+b_cur->exp;
                    c_cur = c->add_term(coef,e,c_cur);
                }
            
                
            }
            end_time = clock();
            // auto end = chrono::high_resolution_clock::now();
            // for(node* c_cur = c.header->next;c_cur!=c.header;c_cur = c_cur->next){
            //     cout<<c_cur->coef<<"x^"<<c_cur->exp;
            //     if(c_cur->next!=c.header) cout<<"+";
            // }
            this_time = (double)(end_time - start_time)/CLOCKS_PER_SEC;
            total_time += this_time;
            delete a;
            delete b;
            delete c;
            

            // auto total_time = chrono::duration_cast<chrono::microseconds>(end - start);
            
        }
        total_time/=1000;
        // cout<<"n = "<<n<<" || time = "<<total_time<<"\n";
        cout<<total_time<<"\n";
    }
    for(int i:x){
        cout<<i<<"\n";
    }
    
}