#include <iostream>
#include <stdexcept>
#include <exception>
using namespace std;

const int defaultSize = 5;
const float defaultValue = 1.0f;

class CppArray {
    public:

    CppArray (int size = defaultSize, float initValue = defaultValue) {//建立陣列
        arr_size = size;
        if(size <= 0) {
            cout << "size can not be zero" << endl;
        }
        cout << "constructor execute" << endl;
        cpp_array = new float[size];
        for (int i = 0; i < size; i++) {
            cpp_array[i] = initValue;
        }
    }

    CppArray (const CppArray& cp2) {
        arr_size = cp2.arr_size;
        cpp_array = new float [arr_size];
        for (int i = 0; i < arr_size; i++) {
            cpp_array[i] = cp2.cpp_array[i];
        }
    }

    CppArray& operator = (const CppArray& cp2) {
        if(this == &cp2) {
            return *this;
        }
        delete[] cpp_array;
        arr_size = cp2.arr_size;
        cpp_array = new float[arr_size];
       
        for (int i =0; i < arr_size; i++) {
            cpp_array[i] = cp2.cpp_array[i];
        }
        return *this;
    }

    float& operator[] (int i) { //實作輸出陣列某個值
        if(i >= arr_size) {
            throw out_of_range("i out of range");
        }
        else if (i < 0) {
            throw out_of_range("i out of range");
        }
        return cpp_array[i];
    }

    friend istream& operator >> (istream& is, CppArray& cp2);
    friend ostream& operator << (ostream& os, CppArray& cp2);

    int GetSize() {
        return arr_size;
    }

    ~CppArray() {
        cout << "destructor execute" << endl;
        delete[] cpp_array;
    }

    private:
    float* cpp_array;
    int arr_size;
};

ostream& operator << (ostream& os, CppArray& cp2) {
    for (int i = 0; i < cp2.arr_size; i++) {
        os << cp2.cpp_array[i];
        if (i < cp2.arr_size-1) {
            os << ", ";
        }
    }
    os << endl;
    return os;
}

istream& operator >> (istream& is, CppArray& cp2) {
    for (int i = 0; i < cp2.arr_size; i++) {
        is >> cp2.cpp_array[i];
    }
    return is;
}

int main() {

    CppArray cp1;
    CppArray cp2(3, 3);
    cout << cp1.GetSize() << endl;
    cp1 = cp2;
    cout << cp1.GetSize() << endl;
    
    cout << cp2;
    cout << cp2;
    try {
        cout << cp1[4] << endl;
    }
    catch (out_of_range& e) {
            cerr << e.what() << endl;
    }
    
    return 0;
}
