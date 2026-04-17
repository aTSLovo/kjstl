#ifndef SHARED_PTR_H
#define SHARED_PTR_H
#include <cstddef>
#include <atomic>
#include <string.h>
#include <iostream>
namespace kj {
bool config = true;
template<class... Args>
void print(Args... args) {
    if(!config) return ;
    (std::cout << ... << args) << '\n';
}

template<class T>
struct DefaultDeleter { // 默认使用 delete 释放内存
    void operator()(T* p) const {
        delete p;
    }
};

template <class _Tp>
struct DefaultDeleter<_Tp[]> { // 偏特化
    void operator()(_Tp *p) const {
        delete[] p;
    }
};

// 控制块基类、也是Shared_ptr指向的指针类型
struct spCounter {
    std::atomic<long> m_ref;

    spCounter() noexcept : m_ref(1) {}

    spCounter(spCounter&&) = delete;

    void incRef() noexcept {
        m_ref.fetch_add(1, std::memory_order_relaxed);
    }

    void decRef() noexcept {
        if(m_ref.fetch_sub(1, std::memory_order_relaxed) == 1) {
            delete this;
        }
    }

    long cntRef() noexcept {
        return m_ref.load(std::memory_order_relaxed);
    }

    virtual ~spCounter() = default;
};

// 具体的实现、原始数据类型+删除器
template<class T, class Deleter>
struct spCounterImpl final : spCounter {
    T* m_ptr;
    [[no_unique_address]] Deleter m_deleter;   // C++20 空类优化如果 Deleter 是空类（如 DefaultDeleter）编译器不占内存

    explicit spCounterImpl(T* ptr) noexcept
        : m_ptr(ptr) {}

    explicit spCounterImpl(T* ptr, Deleter del) noexcept 
        : m_ptr(ptr), m_deleter(std::move(del)) {}

    ~spCounterImpl() noexcept override {
        m_deleter(m_ptr);
    }
};

/**
 * @brief 用于一次性申请对象和控制块内存的make_shared
 * 
 * 控制块只调用指针析构函数，调用operator delete内存释放
 */
template<class T, class Deleter>
struct spCounterImplFused final : spCounter {
    T* m_ptr;
    void* m_mem;
    [[no_unique_address]] Deleter m_deleter;

    explicit spCounterImplFused(T* __ptr, void* __mem,
                                Deleter __deleter) noexcept
        : m_ptr(__ptr),
          m_mem(__mem),
          m_deleter(std::move(__deleter)) {
        
    }

    ~spCounterImplFused() noexcept {
        m_deleter(m_ptr);
    }

    // 引用计数为0时delete this;
    // 内存最后由 Fused 控制块的 operator delete 统一释放，外部删除器只调用析构函数
    void operator delete(void* __mem) noexcept {
// C++ 特性测试宏
#if __cpp_aligned_new
    // 支持 C++17 对齐分配时走这里
        ::operator delete(
            __mem, std::align_val_t(
                        std::max(alignof(T), alignof(spCounterImplFused))));
#else
        ::operator delete(__mem);
#endif
    }
};

template<class T>
class Shared_ptr {
private:
    T* m_ptr;               // 指向被管理的对象
    spCounter* m_owner;     // 指向控制块

    template <class>
    friend class Shared_ptr;    // 模板友元声明, 让Shared_ptr的所有实例化版本互相成为友元、能访问彼此的私有成员

    // 由_S_makeSharedFused完成调用
    explicit Shared_ptr(T* __ptr, spCounter* __owner) noexcept
        : m_ptr(__ptr),
          m_owner(__owner) {}

public:
    Shared_ptr(std::nullptr_t = nullptr) noexcept : m_ptr(nullptr) {}

    // 默认删除器版本
    template<class Y,
             std::enable_if_t<std::is_convertible_v<Y*, T*>, int> = 0>
    explicit Shared_ptr(Y* ptr)
        : m_ptr(ptr),
          m_owner(new spCounterImpl<Y, DefaultDeleter<Y>>(ptr)) {
        // print("Shared_ptr构造函数");
    }

    // 自定义删除器版本
    template<class Y, class Deleter,
             std::enable_if_t<std::is_convertible_v<Y*, T*>, int> = 0>
    explicit Shared_ptr(Y* ptr, Deleter del)
        : m_ptr(ptr),
          m_owner(new spCounterImpl<Y, Deleter>(ptr, std::move(del))) {
    }

    template <class Y>
    inline friend Shared_ptr<Y>
    _S_makeSharedFused(Y *__ptr, spCounter *__owner) noexcept;

    // 拷贝构造，相同类
    Shared_ptr(const Shared_ptr& that) noexcept
        : m_ptr(that.m_ptr),
          m_owner(that.m_owner) {
        if(m_owner) {
            m_owner->incRef();
        }
    }

    /**
     * @brief 泛型拷贝构造
     * 
     * 派生类(要把所有Shared_ptr模板类声明为友元)
     * 类型安全检查（SFINAE）,只有当Y可以隐式转换成T时，这个构造函数才允许生效。
     */
    template<class Y,
             std::enable_if_t<std::is_convertible_v<Y*, T*>, int> = 0 >
    Shared_ptr(const Shared_ptr<Y>& that) noexcept
        : m_ptr(that.m_ptr),
          m_owner(that.m_owner) {
        if(m_owner) {
            m_owner->incRef();
        }
    }

    // 移动构造
    Shared_ptr(Shared_ptr&& that) noexcept
        : m_ptr(that.m_ptr),
          m_owner(that.m_owner) {
        that.m_ptr = nullptr;
        that.m_owner = nullptr;
    }
    
    template<class Y,
             std::enable_if_t<std::is_convertible_v<Y*, T*>, int> = 0 >
    Shared_ptr(Shared_ptr<Y>&& that) noexcept
        : m_ptr(that.m_ptr),
          m_owner(that.m_owner) {
        that.m_ptr = nullptr;
        that.m_owner = nullptr;
    }

    ~Shared_ptr() {
        if(m_owner) {
            m_owner->decRef();
        }
    }
    T& operator*() {
        return *m_ptr;
    }
    T* operator->() {
        return m_ptr;
    }
    T* get() {
        return m_ptr;
    }
};

/**
 * @brief 普通版，new两次，一次创建对象，一次创建控制块
 */
// template<class T, class ...Args>
// Shared_ptr<T> Make_shared(Args&&... args) {
//     return Shared_ptr<T>(new T(std::forward<Args>(args)...));
// }

template <class T>
inline Shared_ptr<T> _S_makeSharedFused(T* __ptr, spCounter* __owner) noexcept {
    return Shared_ptr<T>(__ptr, __owner);
}

template<class T, class... Args,
         std::enable_if_t<!std::is_unbounded_array_v<T>, int> = 0>
Shared_ptr<T> Make_shared(Args&&... args) {
    // 内存是整块申请的，不能 delete 对象。对象和控制块在同一块内存里，必须最后一起释放。
    // 删除器只调用析构，不释放内存。
    auto const __deleter = [](T* __ptr) noexcept {
        __ptr->~T();
    };
    using _Counter = spCounterImplFused<T, decltype(__deleter)>;
    std::size_t __offset = std::max(alignof(T), sizeof(_Counter));
    std::size_t __align = std::max(alignof(T), alignof(_Counter));
    std::size_t __size = __offset + sizeof(T);

#if __cpp_aligned_new
    void* __mem = ::operator new(__size, std::align_val_t(__align));
    _Counter* __counter = reinterpret_cast<_Counter*>(__mem);
#else
#endif

    T* __object =
        reinterpret_cast<T*>(reinterpret_cast<char*>(__counter) + __offset);
    try {
        // 定位new，在__object地址调用构造函数创建对象
        new (__object) T(std::forward<Args>(args)...);
    } catch (...) {
#if __cpp_aligned_new
        ::operator delete(__mem, std::align_val_t(__align));
#else
        ::operator delete(__mem);
#endif
        throw;
    }
    new (__counter) _Counter(__object, __mem, __deleter);
    return _S_makeSharedFused(__object, __counter);
}

};

#endif