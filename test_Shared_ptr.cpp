#include "Shared_ptr.h"
#include <iostream>
using namespace kj;
class Test {
public:
    int val_;
    std::string s_;

    Test() = default;
    Test(int val, std::string s) : val_(val), s_(s) {
        std::cout << "Test 构造函数" << '\n';
    }
    ~Test() {
        std::cout << "Test 析构函数" << '\n';
    }
};
class TestDerived : public Test {
public:
    explicit TestDerived(int val, std::string s) : Test(val, s) {
        std::cout << "TestDerived 构造函数" << '\n';
    }
    ~TestDerived() {
        std::cout << "Test 析构函数" << '\n';
    }
};
int main() {
    Shared_ptr<Test> test1 = Make_shared<Test>(1, "abcde");     // make_shared一次性创建
    print("make_shared一次性创建: ", (*test1).val_);
    Shared_ptr<Test> test2(new Test(2, "abc"));                 // 用new创建指针后再构造
    print("用new创建指针后再构造: ", (*test2).val_);
    Shared_ptr<Test> test3 = test1;                             // 拷贝构造、同类型
    print("拷贝构造、同类型: ", (*test3).val_);
    Test ptr = *test1;                                          // 解引用
    print("operator*测试: ", ptr.s_);
    Shared_ptr<Test> test4(new Test(4, "cdcd"),                 // 自定义删除器
        [] (Test* p) { 
            print("lambda删除器");
            delete p; 
    });   // 自定义deleter
    print("自定义删除器测试：", (*test4).val_);
    Shared_ptr<Test> test5(new TestDerived(5, "axd"));          // 派生类构造
    print("派生类构造：", (*test5).val_);
    Shared_ptr<Test> test6                                      // 拷贝构造、派生类
        = Make_shared<TestDerived>(6, "xv");
    print("拷贝构造、派生类: ", (*test6).val_);
    return 0;
}