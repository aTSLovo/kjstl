#include <memory>
#include <typeinfo>

namespace kj {
// 主模板的声明中不允许使用模板参数列表
template<typename>
class Function;

/**
 * @brief function<xxx>的底层实现
 * 
 * _m_base指向FuncImpl类型擦除后的对象
 */
template<typename Ret, typename ...Args>
class Function<Ret(Args...)> {
public:
    // 默认构造：空Function
    Function() noexcept = default;

    // 空指针构造：等价于默认构造
    Function(std::nullptr_t) noexcept : Function() {}

    // 模板构造：接收任意可调用对象
    template<typename Fn>
        // 编译期间判断一个函数对象是否可以使用指定的参数调用
        requires(std::is_invocable_r_v<Ret, std::decay_t<Fn>, Args...>
        && !std::is_same_v<std::decay_t<Fn>, Function<Ret(Args...)>>)
    Function(Fn&& fn) : _m_base(std::make_unique<FuncImpl<std::decay_t<Fn>>>(std::forward<Fn>(fn)))
    {}

    // 拷贝构造
    Function(const Function& that) : _m_base(that._m_base ? that.target_type->clone() : nullptr)
    {}

    Ret operator()(Args... args) const {
        return _m_base->call(std::forward<Args>(args)...);
    }
    
    const std::type_info& target_type() const {
        return _m_base ? _m_base->target_type() : typeid(void);
    }
private:
    // 写在类中：模板参数复用
    // 抽象基类（定义统一接口，擦除具体类型）
    struct FuncBase {
        virtual Ret call(Args... args) = 0; // 调用接口
        virtual std::unique_ptr<FuncBase> clone() const = 0;    
        virtual const std::type_info& target_type() const = 0;  // 类型查询接口
        virtual ~FuncBase() = default;
    };

    // Func实现类（封装具体可调用对象Fn _m_f，实现基类接口）
    template<typename Fn>
    struct FuncImpl : FuncBase {
        Fn _m_f;

        template<typename ...CArgs>
        FuncImpl(CArgs... cargs) : _m_f(std::forward<CArgs>(cargs)...) {}
        
        // 完美转发所有参数给构造时保存的仿函数对象：
        Ret call(Args... args) override {
            return _m_f(std::forward<Args>(args)...);
        }
        // 返回的是类型为FuncImpl的unique_ptr，但是指向FuncImpl指针可以隐式转换指向FuncBase
        std::unique_ptr<FuncBase> clone() const override {
            return std::make_unique<FuncImpl>(_m_f); // 不能实例化抽象基类FuncBase
        }
        const std::type_info& target_type() const override {
            return typeid(Fn);
        }
    };

    std::unique_ptr<FuncBase> _m_base;
};
};