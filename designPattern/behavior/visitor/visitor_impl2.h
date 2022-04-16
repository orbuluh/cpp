#pragma once
#include <iostream>
namespace visitor::impl2 {
// impl2 -> actual behavior defined in each visitable
// Reference: [stackoverflow](https://stackoverflow.com/a/11802080/4924135)


template <typename Derived>
struct Visitable {
    void accept() {
        static_cast<Derived*>(this)->accept();
    }
};

struct Residential : public Visitable<Residential> {
    void accept() {
        std::cout << "Welcome home.\n";
    }
};
struct Bank : public Visitable<Bank> {
    void accept() {
        std::cout << "Welcome customer, what bank service do you need?\n";
    }
};

struct CoffeeShop : public Visitable<CoffeeShop> {
    void accept() {
        std::cout << "Welcome customer, what coffee would you like\n";
    }
};


// variadic template declaration for sfinae
template<typename... Types>
class Visitor;

// specialization for single type
template<typename T>
class Visitor<T> {
public:
    void visit(T& visitable) {
        visitable.accept();
    };
};

// specialization for varios type
template<typename T, typename... Types>
class Visitor<T, Types...> : public Visitor<Types...> {
public:
    using Visitor<Types...>::visit;
    void visit(T& visitable) { visitable.accept(); }
};

struct Customer : Visitor<Residential, Bank, CoffeeShop> {};

} // namespace visitor::impl2