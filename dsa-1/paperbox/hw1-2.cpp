#include<iostream>
#include <string>
#include <cstring>
using namespace std;

int main() {
    int n;
    int longest = 0;
    cin >> n;
    int firstspace[n];//存每個名字的第一個空格
    int thespace=0;//存最右邊的空格在哪裡
    string* namelist = new string[n];
    cin.ignore();
    for (int i=0;i<n;i++) {
        getline(cin, namelist[i]);
        int len = namelist[i].length();
        if(len>=longest) {
            longest = len;
        }
        firstspace[i] = namelist[i].find(" ");//找到第一個空格
        if (firstspace[i]>thespace) {
            thespace = firstspace[i];
        }
    }
    char** list = new char*[n];//創二維陣列來存
    for(int i=0;i<n;i++) {
        list[i] = new char[longest+1];
        strcpy(list[i], namelist[i].c_str());
    }
    for (int i=0;i<n;i++) {
        for(int j=0;j<thespace-firstspace[i];j++){
            cout << " ";
        }
        cout << list[i] << endl;
    }

    for (int i=0; i<n; i++) {
        delete []list[i];
    }
    delete []list;
    delete []namelist;
    return 0;
}