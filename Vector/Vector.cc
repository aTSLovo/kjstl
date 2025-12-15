#include "Vector.h"
#include <iostream>
#include <cstring>
#include <algorithm>
namespace kj {
Vector::Vector() {
    m_data = nullptr;
    m_size = 0;
}
Vector::Vector(size_t n) {
    m_data = new int[n]{};
    m_size = n;
}
Vector::~Vector() {
    delete[] m_data;
}

int Vector::operator[](size_t i) const {
    return m_data[i];
}
int& Vector::operator[](size_t i) {
    return m_data[i];
}

size_t Vector::size() const { return m_size; }
void Vector::resize(size_t n) {
    int* old_data = m_data;
    size_t old_size = m_size;
    m_data = new int[n]{};
    m_size = n;
    if(old_data) {
        memcpy(m_data, old_data, std::min(n, old_size) * sizeof(int));
        delete[] old_data;
    }
}
}
