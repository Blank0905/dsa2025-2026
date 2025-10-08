#include<iostream>
#include<cstring>
using namespace std;

int main() {
    int x = 0;
    for(int i = 1;i <= 2;i++) {
        for(int j = 1;j<=i;j ++) 
            for(int k = 1;k <= j;k++)
                x++;
    }
    cout << x;
}