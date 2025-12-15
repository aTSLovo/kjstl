#pragma once
#include <cstdio>
#include <memory>
namespace kj{
template<class T>
class Vector {
private:
    T* m_data;
    std::size_t m_size;
    std::size_t m_capacity;
    std::allocator<T> alloc;
    void reserve(size_t n) {
        if(n <= m_capacity) return;
        n = std::max(n, m_capacity * 2);
        T* old_data = m_data;
        std::size_t old_cap = m_capacity;
        if(n == 0) {
            m_data = nullptr;
            m_capacity = 0;
        }
        else {
            m_data = alloc.allocate(n);
            m_capacity = n;
        }
        if(old_cap != 0) {
            for(size_t i = 0; i < m_size; ++i) {
                std::construct_at(&m_data[i], old_data[i]);
            }
            for(size_t i = 0; i < m_size; ++i) {
                std::destroy_at(&old_data[i]);
            }
            alloc.deallocate(old_data, old_cap);
        }
    }
public:
    //默认构造函数，适合不带值初始化Vector arr;
    Vector() : m_data(nullptr), m_size(0), m_capacity(0) {}
    //构造函数,explicit禁止隐式类型转换
    explicit Vector(size_t n) {
        m_data = alloc.allocate(n);
        for(size_t i = 0; i < n; ++i) {
            std::construct_at(&m_data[i], T());
        }
        m_size = n;
        m_capacity = n;
    }
    ~Vector() {
        for(size_t i = 0; i < m_size; ++i) {
            std::destroy_at(&m_data);
        }
        alloc.deallocate(m_data, m_capacity);
    }

    T operator[](size_t i) const {
        return m_data[i];
    }
    T& operator[](size_t i) {
        return m_data[i];
    }

    size_t size() const {
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
    T& back() {
        return m_data[m_size - 1];
    }
    const T& back() const {
        return m_data[m_size - 1];
    }
    void pop_back() {
        if(m_size > 0) {
            std::destroy_at(&m_data[m_size - 1]);
            --m_size;
        }
    }
};
}