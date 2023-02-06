#Boost playground
[Boost C++ Application Development Cookbook](https://www.packtpub.com/product/boost-c-application-development-cookbook/9781849514880)

# Boost.Any
* The Boost.Any library was accepted into C++17 `<any>`
* Such flexibility never comes without any cost.
    * Copy constructing, value constructing, copy assigning, and assigning values to instances of boost::any do dynamic memory allocation
    * `boost::any` uses virtual functions a lot.
    * `boost::any` makes use of rvalue references but can not be used in `constexpr`.
* How it works... type erasure technique:

```cpp
struct placeholder {
    virtual ~placeholder() {}
    virtual const std::type_info& type() const = 0;
};
template<typename ValueType>
struct holder : public placeholder {
    virtual const std::type_info& type() const {
        return typeid(ValueType);
        // any_cast<T> will eventually call this and compare with type(T)
        // and if it's the same type, return `static_cast<holder<T>*>(ptr)->held`
    }
    ValueType held;
};
```