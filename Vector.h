#pragma once
#include <cstdio>
#include <memory>
namespace kj{
template<class T, class Allocator = std::allocator<T>>
class Vector {
private:
    T* m_data;
    std::size_t m_size; // 装载元素数量
    std::size_t m_capacity; // 容量
    Allocator m_alloc;
    
public:
    //默认构造函数，适合不带值初始化Vector arr;
    Vector() : m_data(nullptr), m_size(0), m_capacity(0) {}
    //构造函数,explicit禁止隐式类型转换
    explicit Vector(size_t n) {
        m_data = m_alloc.allocate(n);
        for(size_t i = 0; i < n; ++i) {
            std::construct_at(&m_data[i], T());
        }
        m_size = n;
        m_capacity = n;
    }
    Vector& operator=(const Vector& that) {
        if(&that == this) return *this;
        reserve(that.m_size);
        m_size = that.m_size;
        for(size_t i = 0; i != m_size; ++i) {
            std::construct_at(&m_data[i], that.m_data[i]);
        }
        return *this;
    }
    // 右值版本的，释放自身内存，然后接管that
    Vector& operator=(Vector&& that) {
        if(&that == this) return *this;
        for(size_t i = 0; i != m_size; ++i) {
            std::destroy_at(&m_data[i]);
        }
        if(m_capacity != 0) {
            m_alloc.deallocate(m_data, m_capacity);
        }
        m_size = that.m_size;
        m_capacity = that.m_capacity;
        m_data = that.m_data;
        that.m_size = 0;
        that.m_capacity = 0;
        that.m_data = nullptr;
        return *this;
    }
    void assign(size_t count, const T& val) {
        clear();
        reserve(count);
        m_size = count;
        for(size_t i = 0; i != count; ++i) {
            std::construct_at(&m_data[i], val);
        }
    }
    // 迭代器版本，
    template<std::random_access_iterator InputIt>
    void assign(InputIt first, InputIt last) {
        clear();
        size_t new_size = last - first;
        reserve(new_size);
        m_size = new_size;
        for(size_t i = 0; i != new_size; ++i) {
            std::construct_at(&m_data[i], *first);
            ++first;
        }
    }
    ~Vector() {
        for(size_t i = 0; i < m_size; ++i) {
            std::destroy_at(&m_data[i]);
        }
        m_alloc.deallocate(m_data, m_capacity);
    }
    // 预留存储空间
    void reserve(size_t new_cap) {
        if(new_cap <= m_capacity) return;
        new_cap = std::max(new_cap, m_size * 2);
        T* old_data = m_data;
        std::size_t old_cap = m_capacity;
        if(new_cap == 0) {
            m_data = nullptr;
            m_capacity = 0;
        }
        else {
            m_data = m_alloc.allocate(new_cap);
            m_capacity = new_cap;
        }
        if(old_cap != 0) {
            for(size_t i = 0; i < m_size; ++i) {
                std::construct_at(&m_data[i], old_data[i]);
            }
            for(size_t i = 0; i < m_size; ++i) {
                std::destroy_at(&old_data[i]);
            }
            m_alloc.deallocate(old_data, old_cap);
        }
    }

    const T& operator[](size_t i) const noexcept {
        return m_data[i];
    }
    T& operator[](size_t i) noexcept {
        return m_data[i];
    }
    T& at(size_t i) {
        if(i >= m_capacity) throw std::out_of_range("Vector::at");
        return m_data[i];
    }
    const T& at(size_t i) const {
        if(i >= m_capacity) throw std::out_of_range("(const)Vector::at");
        return m_data[i];
    }

    size_t size() const noexcept {
        return m_size;
    }
    void resize(size_t n) {
        if(n < m_size) {
            for(size_t i = n; i < m_size; ++i) {
                std::destroy_at(&m_data[i]);
            }
        }
        else if(n > m_size) {
            reserve(n);
            for(size_t i = m_size; i < n; ++i) {
                std::construct_at(&m_data[i], T());
            }
        }
        m_size = n;
    }

    T& front() noexcept {
        return m_data[0];
    }
    const T& front() const noexcept {
        return m_data[0];
    }
    T& back() {
        return m_data[m_size - 1];
    }
    const T& back() const {
        return m_data[m_size - 1];
    }
    // 从容器中擦除所有元素，size置0，不改变容器容量
    void clear() {
        for(int i = 0; i != m_size; ++i) {
            std::destroy_at(&m_data[i]);
        }
        m_size = 0;
    }
    void push_back(const T& val) {
        if(m_size == m_capacity) {
            reserve(m_size + 1);
            ++m_size;
        }
        m_data[++m_size] = val;
    }
    void push_back(T&& val) {
        if(m_size == m_capacity) {
            reserve(m_size + 1);
            ++m_size;
        }
        m_data[++m_size] = val;
    }
    void pop_back() {
        if(m_size > 0) {
            std::destroy_at(&m_data[m_size - 1]);
            --m_size;
        }
    }
};
}