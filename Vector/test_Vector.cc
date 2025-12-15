#include "Vector.h"
#include <iostream>

int main() {
    kj::Vector<int> arr(3);
    for(size_t i = 0 ; i < arr.size(); ++i) {
        arr[i] = i;
    }
    arr.resize(4);
    //std::cout << arr.size() << '\n';
    for(size_t i = 0 ; i < arr.size(); ++i) {
        //arr[i] = i;
        std::cout << arr[i] << '\n';
    }
    return 0;
}