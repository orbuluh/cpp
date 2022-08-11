#include <iostream>
#include <memory>
namespace crtp {
template <typename T> class Base {
  public:
    T& impl() { return static_cast<T&>(*this); }
    const T& impl() const { return static_cast<const T&>(*this); }
    void saySomething() const { impl().say(); }
    void changeSth() { impl().change(); }
    void doStatic() { T::static_fnc(); }

  protected:
    int thoughtIdx_ = 0;
};

class DerivedYo : public Base<DerivedYo> {
  public:
    void say() const { std::cout << "yo\n"; }
    void change() {
        thoughtIdx_ = 8;
        std::cout << thoughtIdx_ << '\n';
    }
    static void static_fnc() { std::cout << "I have static fnc\n"; }
};

class DerivedOk : public Base<DerivedOk> {
  public:
    void say() const { std::cout << "ok\n"; }
    void change() {
        thoughtIdx_ = 24;
        std::cout << thoughtIdx_ << '\n';
    }
};

void demo() {
    DerivedYo yo;
    yo.saySomething();
    yo.changeSth();
    yo.doStatic();
    DerivedOk ok;
    ok.saySomething();
    ok.changeSth();
    //ok.doStatic(); // didn't define, can't compile!
}

} // namespace crtp