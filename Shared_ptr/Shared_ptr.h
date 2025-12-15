#ifndef SHARED_PTR_H
#define SHARED_PTR_H
namespace kj {
template<class T>
class Shared_ptr {

private:
    T* ptr;
    int ref;
};
}
#endif