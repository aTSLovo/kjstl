#include <iostream>
#include <vector>
struct MoveMsg {
    int x;
    int y;
    void speak() {
        std::cout << "Move " << x << ',' << y << '\n';
    }
};
struct JumpMsg {
    int height;
    void speak() {
        std::cout << "Jump " << height << '\n';
    }
};
struct SleepMsg {
    int time;
    void speak() {
        std::cout << "Sleep " << time << '\n';
    }
};
struct ExitMsg {
    void speak() {
        std::cout << "Exit" << '\n';
    }
};

struct MsgBase {
    virtual void speak() = 0;
    ~MsgBase();
};

template<class T>
struct MsgImpl : MsgBase {
    T msg;

    template<class ..._Args>
    MsgImpl(_Args&&... _args) : msg(std::forward<_Args>(_args)...) {}

    void speak() override {
        msg.speak();
    }
    ~MsgImpl();
};
// 目的是把没有用多态继承的类能统一存入一个类的容器中
// 一个个类重写继承也太麻烦了，使用类型擦除技术
std::vector<MsgBase*> msgs;

int main() {
    msgs.push_back(new MsgImpl<MoveMsg>{1, 2});
    msgs.push_back(new MsgImpl<JumpMsg>{5});
    msgs.push_back(new MsgImpl<SleepMsg>{10});
    msgs.push_back(new MsgImpl<ExitMsg>{});

    for(auto& msg : msgs) {
        msg->speak();
    }
    return 0;
}