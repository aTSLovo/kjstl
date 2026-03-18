#include <iostream>
#include <type_traits>
namespace kj {
// move实际不移动任何东西，只确保move后的东西是个右值
// // c++11
// template<typename T>
// typename std::remove_reference<T>::type&& move(T&& param) {
//     using ReturnType = typename std::remove_reference<T>::type&&;
//     return static_cast<ReturnType>(param);
// }
// c++14
template<typename T>
decltype(auto) move(T&& param) {
    /*
    这里T&&是万能引用，实际根据初始化用的是左右值推导为int&或int
    通过std::remove_reference_t移除T的引用性，这样ReturnType就是一个右值了(T不用推导嘛)
    最后通过static_cast将param强制转化为右值
    move的返回值用了decltype(auto)，防止auto产生临时对象(auto会忽略引用性，推导为值类型)，避免值拷贝(因为返回是个右值引用)
    */
    using ReturnType = typename std::remove_reference_t<T>&&;
    return static_cast<ReturnType>(param);
}
};

int main() {
    int a = 5;
    int b = kj::move(a);
    std::cout << "a: " << a << '\n';
    std::cout << "b: " << b << '\n';
}
