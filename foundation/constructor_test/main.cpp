#include <iostream>

class A {
public:
    A() { std::cout << 1; }
    ~A() { std::cout << 2; }
};

class B {
public:
    B() { std::cout << 3; }
    ~B() { std::cout << 4; }
protected:
    A a;
};

class C : public B {
public:
    C() { std::cout << 5; }
    ~C() { std::cout << 6; }
private:
    A a;
};

int main() {
    C c;
    return 0;
}
// 13156242