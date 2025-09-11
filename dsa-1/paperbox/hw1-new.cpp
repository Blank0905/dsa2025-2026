#include<iostream>
#include<cstring>
using namespace std;

int main() {
    int n, m; //n幾個名字，m名字多長
    cin >> n >> m;
    char **column = new char*[n]; //直的那行指標存指標
    for (int i =0; i< n; i++) {
        column[i] = new char[m+1];//留一格給\0
    }
    cin.ignore();
    for (int i=0;i<n ; i++) {
        cin.getline(column[i], m+1);   
    }
    for(int j = n; j>0; j--) {
        for(int i=0; i<j-1; i++) {
            int len1, len2;
            len1=strlen(column[i]);
            len2=strlen(column[i+1]);
            if(column[i][len1-1]>column[i+1][len2-1]) {
                char *temp = column[i];
                column [i] = column [i+1];
                column [i+1] = temp;
            }
        }
    }
    cout << "\n";
    for (int i=0;i<n ; i++) {
        cout << column[i] << "\n";
    }

    for(int i = 0; i <n; i++) {
        delete []column[i];
    }
    delete []column;
    return 0;
}
