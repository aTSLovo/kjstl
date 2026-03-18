#include "Vector.h"
#include <iostream>
template<class T>
void print_Vector(kj::Vector<T>& arr) {
    for(size_t i = 0; i != arr.size(); ++i) {
        std::cout << arr[i] << ' ';
    }
    std::cout << '\n';
}
int main() {
    kj::Vector<int> arr(3);
    for(size_t i = 0 ; i < arr.size(); ++i) {
        arr[i] = i;
    }
    std::cout << arr.at(2) << '\n';
    try {
        std::cout << arr.at(3) << '\n';
    } catch (const std::out_of_range& ex) {
        std::cout << ex.what() << '\n';
    }
    
    std::cout << "resize() before " << arr.size() << '\n';
    arr.resize(4);
    std::cout << "resize() after " << arr.size() << '\n';
    print_Vector(arr);
    std::cout << "assign(count, val)" << '\n';
    arr.assign(3, 10);
    print_Vector(arr);
    std::cout << "assign(InputIt first, InputIt last)" << '\n';
    const std::string extra(3, 'A');
    kj::Vector<char> iterator_assign;
    iterator_assign.assign(extra.begin(), extra.end());
    print_Vector(iterator_assign);
    
    return 0;
}