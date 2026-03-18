#include "Function.h"
#include <functional>
#include <iostream>
#include <memory>
#include <typeinfo>
void print_num(int i)
{
    std::cout << i << '\n';
}

int main() {
    // store a free function
    kj::Function<void(int)> f_printnum = print_num;
    f_printnum(8);
    
    std::cout << "print_num函数地址：" << (void*)print_num << '\n';
    // 打印f_printnum的对象地址（栈地址）
    std::cout << "f_printnum对象地址：" << &f_printnum << '\n';

    std::cout << "f_printnum类型" << f_printnum.target_type().name() << '\n';
    // store a lambda
    kj::Function<void()> f_lambda = []() { std::cout << "lambda" << '\n'; };
    f_lambda();
    std::cout << "f_lambda类型" << f_lambda.target_type().name() << '\n';
    // store the result of a call to std::bind
    kj::Function<void()> f_display_31337 = std::bind(print_num, 31337);
    f_display_31337();
    std::cout << "f_display_31337类型" << f_display_31337.target_type().name() << '\n';
    return 0;
}