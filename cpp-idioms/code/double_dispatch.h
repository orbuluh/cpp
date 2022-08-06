#include <iostream>
#include <memory>
namespace single_dispatch {
// e.g. just normal virtual function
// the function that is actually called is determined by a single factor,
// the type of the object.
class Base {
    virtual void f() = 0;
};
class Derived1 : public Base {
    void f() override { std::cout << "D1\n"; }
};
class Derived2 : public Base {
    void f() override { std::cout << "D2\n"; }
};

} // namespace single_dispatch

namespace double_dispatch {

// when calling b->f(p)
// both the *b and *p object can be of either of the two derived types.
// the code that runs, in the end, is determined by two separate factors.

class Base {
public:
    virtual ~Base() = default;
    virtual void f(Base*) = 0;
};
class Derived1 : public Base {
public:
    void f(Base* b) override { std::cout << "D1\n"; }
};
class Derived2 : public Base {
public:
    void f(Base* b) override { std::cout << "D2\n"; }
};

void demo() {
    std::unique_ptr<Base> p(new Derived1());
    p->f(p.get());
}

} // namespace double_dispatch