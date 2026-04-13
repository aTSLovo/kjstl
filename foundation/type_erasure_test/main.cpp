#include "msglib.h"
#include <memory>
#include <vector>
#include <map>

/**
 * @brief 外挂函数 msg_extra_funcs::load() —— 非侵入式扩展
 * 
 * 解决
 * 原始消息结构体不能修改（比如来自第三方库、不能加成员函数）
 * 用重载函数外挂扩展功能，不侵入原有代码
 */
namespace msg_extra_funcs { // 无法为 Msg 们增加成员函数，只能以重载的形式，外挂追加
    void load(MoveMsg &msg) {
        std::cin >> msg.x >> msg.y;
    }

    void load(JumpMsg &msg) {
        std::cin >> msg.height;
    }

    void load(SleepMsg &msg) {
        std::cin >> msg.time;
    }

    void load(ExitMsg &) {

    }
}

/**
 * @brief 基类接口 MsgBase —— 多态的核心
 * 
 * MsgBase 指向 MsgImpl<MoveMsg>，而 MsgImpl 里面装着 MoveMsg
 * 基类指针可以指向任意子类对象MsgImpl<Msg（Move/Jump/Sleep/Exit）>
 * 用 shared_ptr 管理对象生命周期，避免内存泄漏
 */
struct MsgBase {
    virtual void speak() = 0;
    virtual void load() = 0;
    virtual ~MsgBase() = default;

    using Ptr = std::shared_ptr<MsgBase>;  // 智能指针别名
};

/**
 * @brief 模板类 MsgImpl<Msg> —— 自动适配所有消息
 * 
 * 模板 + 多态：一段代码适配所有消息类型
 * 自动实现基类接口，把调用转发给具体消息
 * 不用给每个消息写一遍继承，代码极度精简
 */
template<class T>
struct MsgImpl : MsgBase {
    T msg;  // Impl实现类，里面藏了 MoveMsg

    // 构造函数
    // template<class ..._Args>
    // MsgImpl(_Args&&... _args) : msg(std::forward<_Args>(_args)...) {}

    void speak() override {
        msg.speak();
    }

    void load() override {
        msg_extra_funcs::load(msg);
    }
};

template<class Msg, class...Args>
std::shared_ptr<MsgBase> makeMsg(Args&&... args) {
    return std::make_shared<MsgImpl<Msg>>(std::forward<Msg>(args)...);
}

class MsgFactoryBase {
public:
    virtual MsgBase::Ptr create() = 0;
    virtual ~MsgFactoryBase() = default;

    using Ptr = std::shared_ptr<MsgFactoryBase>;
};

template<class Msg>
class ImplMsgFactory : public MsgFactoryBase{
    MsgBase::Ptr create() override {
        return std::make_shared<MsgImpl<Msg>>();
    }
};

template<class Msg>
MsgFactoryBase::Ptr makeMsgFac() {
    return std::make_shared<ImplMsgFactory<Msg>>();
}

class Robot {
private:
    // 消息注册表
    inline static const std::map<std::string, MsgFactoryBase::Ptr> factories = {
        {"Move", makeMsgFac<MoveMsg>()},
        {"Jump", makeMsgFac<JumpMsg>()},
        {"Sleep", makeMsgFac<SleepMsg>()},
        {"Exit", makeMsgFac<ExitMsg>()}
    };
public:
    void recv_data() {
        std::string type;
        std::cin >> type;

        // 扩展消息麻烦，使用工厂模式创建对象
        // if(type == 1) {
        //     msg_ = makeMsg<MoveMsg>();
        // }
        try {
            msg_ = factories.at(type)->create();
        } catch(std::out_of_range e) {
            std::cout << "no such msg type!\n";
            return;
        }
        msg_->load();
    }

    void update() {
        if (msg_)
            msg_->speak();
    }

private:
    MsgBase::Ptr msg_;
};

/**
 * 类型擦除：MoveMsg、JumpMsg等无关的结构体不能放在一个数组中、不能用同一个map存储、不能用同一个指针指向
 * 用继承和虚函数把类型隐藏起来
 * 外界只看到基类，看不到真实子类
 * 真实类型被 “擦掉” 了，看不见了。
 * 
 * 目的是把没有用多态继承的类能统一存入一个类的容器中
 * 一个个类重写继承也太麻烦了，使用类型擦除技术
 * 然后用工厂模式根据输入消息类型创建对象
 */
int main() {
    Robot robot;
    robot.recv_data();
    robot.update();
    return 0;
}