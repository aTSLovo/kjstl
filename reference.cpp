#include <iostream>
struct printnum_args_t {
    int& x; // 引用形式，在外部修改初始化的值，这里也会更改
    int& y;
};
void func_printnum(void* args) {
    auto a = (printnum_args_t*) args;
    std::cout << "Numbers are:" << a->x << ',' << a->y << '\n';
}
typedef void (*pfunc_t)(void* args);
void repeattwice(pfunc_t func, void* args) {
    func(args);
    func(args);
}
int main() {
    int x, y;
    std::cin >> x >> y;
    printnum_args_t arg{x, y};
    std::cout << (&x) << ' ' << (&arg.x) << '\n';
    repeattwice(func_printnum, &arg);
    std:: cin >> x >> y;
    repeattwice(func_printnum, &arg);
}