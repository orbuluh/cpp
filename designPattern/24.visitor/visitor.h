#pragma once

namespace visitor {

// Reference: [stackoverflow](https://stackoverflow.com/a/11802080/4924135)

// variadic template declaration for sfinae
template<typename... Types>
class Visitor;

// specialization for single type
template<typename T>
class Visitor<T> {
public:
    virtual void visit(T& visitable) = 0;
};

// specialization for varios type
template<typename T, typename... Types>
class Visitor<T, Types...> : public Visitor<Types...> {
public:
    using Visitor<Types...>::visit;
    virtual void visit(T& visitable) = 0;
};

template<typename... Types>
class Visitable { // The visitable
public:
    virtual void accept(Visitor<Types...>& visitor) = 0;
};

template<typename Derived, typename... Types>
class VisitableImpl : public Visitable<Types...> {
public:
    virtual void accept(Visitor<Types...>& visitor) {
        visitor.visit(static_cast<Derived&>(*this));
    }
};

void demo();
} // namespace visitor