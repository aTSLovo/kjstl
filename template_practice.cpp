#include <iostream>

// 推导数组含有的元素个数
template<typename T, std::size_t N>
constexpr std::size_t arraySize(T (&)[N]) noexcept {
    return N;
}
// auto可以推导花括号为std::initializer_list，但是模板类型推导不出
template<typename T>
void function_template_braced(std::initializer_list<T> list) {
    std::cout << "传递花括号给模板参数成功:" << '\n';
}
int main() {
    const char s[] = "kljapple";
    std::cout << "字符串数组大小" << arraySize(s) << '\n';
    function_template_braced({1, 2, 3, 4, 5});
    return 0;
}