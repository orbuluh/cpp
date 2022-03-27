#pragma once
#include <typeinfo>
#include <iostream>
namespace proxy {


template <typename T>
struct Property {
    T value_;
    Property(const T initialVal) {
        std::cout << "Invoke ctor for T=" << typeid(T).name() << " val=" << initialVal << '\n';
        *this = initialVal;
    }
    operator T() {
        std::cout << "Invoke T() for T=" << typeid(T).name() << " val=" << value_ << '\n';
        return value_;
    }
    T operator=(T newVal) {
        return value_ = newVal;
    }
};

void demo();
} // namespace proxy