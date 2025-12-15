#include <bits/stdc++.h>
using namespace std;
struct Node {
    int data;
    Node* next;
};

int main() {
    Node *root = nullptr;
    int a = 10;
    int* ptr = &a; // ptr 是指向 a 的指针
    int** doublePtr = &ptr;
    int value = *ptr; // 解引用 ptr，得到 a 的值
    cout << ptr << endl;
    cout << *ptr << endl;
    cout << doublePtr << endl;
    cout << *doublePtr << '=' << ptr << endl;
    cout << **doublePtr << endl;
    std::cout << value << std::endl; // 输出 10
    return 0;
}
