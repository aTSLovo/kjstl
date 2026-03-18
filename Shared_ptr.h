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

/*
    https://www.bilibili.com/video/BV15eWZeuEsV/?spm_id_from=333.1387.search.video_card.click&vd_source=638ed77eaa63e2e7027f7571c6ef757e
    视频学至24分钟、static_pointer_cast
*/
template<class T>
struct DefaultDeleter {
    void operator()(T* p) const {
        delete p;
    }
};
// 控制块基类、也是Shared_ptr指向的指针类型
struct spCounter {
    std::atomic<long> m_ref;

    spCounter() noexcept : m_ref(1) {}
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
    virtual ~spCounter() {}
};
// 具体的实现、原始数据类型+删除器
template<class T, class Deleter>
struct spCounterImpl final : spCounter {
    T* m_ptr;
    Deleter m_deleter;

    explicit spCounterImpl(T* ptr) noexcept
        : m_ptr(ptr) {}

    explicit spCounterImpl(T* ptr, Deleter del) noexcept 
        : m_ptr(ptr), m_deleter(std::move(del)) {}

    ~spCounterImpl() override {
        m_deleter(m_ptr);
    }
};

template<class T>
class Shared_ptr {
private:
    T* m_ptr;
    spCounter* m_sp;

    template <class>
    friend class Shared_ptr;    // 模板友元声明, 让Shared_ptr的所有实例化版本互相成为友元、能访问彼此的私有成员

public:
    Shared_ptr(std::nullptr_t = nullptr) : m_ptr(nullptr) {}

    // 默认删除器版本
    template<class Y>
    explicit Shared_ptr(Y* ptr) noexcept
        : m_ptr(ptr),
          m_sp(new spCounterImpl<Y, DefaultDeleter<Y>>(ptr)) {
        // print("Shared_ptr构造函数");
    }

    // 自定义删除器版本
    template<class Y, class Deleter>
    explicit Shared_ptr(Y* ptr, Deleter del) noexcept 
        : m_ptr(ptr),
          m_sp(new spCounterImpl<Y, Deleter>(ptr, del)) {}

    // 拷贝构造，相同类
    Shared_ptr(const Shared_ptr& that) noexcept
        : m_ptr(that.m_ptr),
          m_sp(that.m_sp) {
        if(m_sp) {
            m_sp->decRef();
        }
    }

    // 拷贝构造，派生类(要把所有Shared_ptr模板类声明为友元)
    template<class Y>
    Shared_ptr(const Shared_ptr<Y>& that) noexcept
        : m_ptr(that.m_ptr),
          m_sp(that.m_sp) {
        if(m_sp) {
            m_sp->incRef();
        }
    }

    ~Shared_ptr() {
        if(m_sp) {
            m_sp->decRef();
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

template<class T, class ...Args>
Shared_ptr<T> Make_shared(Args&&... args) {
    return Shared_ptr<T>(new T(std::forward<Args>(args)...));
}
};
#endif