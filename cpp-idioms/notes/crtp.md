
# Quick fact
- Forms:
  - Inheriting from a template class
  - Use the derived class itself as a template parameter of the base class.
- Why doing so?
  - So you can use derived class in the base class.
  - Imagining: you can build the customized behavior in derived class, and put the logic skeleton in base class.

# Neat way to avoid two classes happen to derive from the same CRTP base class
- from Marek Kurdej as mentioned in the [fluent C++ blog](https://www.fluentcpp.com/2017/05/12/curiously-recurring-template-pattern/)
```cpp
template <typename T>
class Base
{
public:
    // ...
private:
    Base(){};
    friend T;
};

class Derived1 : public Base<Derived1> {}; //... ok
// prevent ...
class Derived2 : public Base<Derived1> {}; // bad, but guard by the friend T in base!
```

extract out a type independent but type customizable functionality in a base class and to mix-in that interface/property/behavior into a derived class, customized for the derived class.


# Reference
- [More C++ Idioms](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Curiously_Recurring_Template_Pattern)
- [fluent C++](https://www.fluentcpp.com/2017/05/12/curiously-recurring-template-pattern/)