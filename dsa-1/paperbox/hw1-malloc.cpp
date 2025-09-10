#include<iostream>
#include<cstring>
using namespace std;

int main() {
    int n, m;
    cin >> n >> m;
    char **column =(char**)malloc(n* sizeof(char*));
    for (int i =0; i< n; i++) {
        column[i] =(char*) malloc((m+1)*sizeof(char));//留一格給\0
    }
    for (int i=0;i<n ; i++) {
        cin >> column[i];    
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
        free(column[i]);
    }
    free(column);
    return 0;
}