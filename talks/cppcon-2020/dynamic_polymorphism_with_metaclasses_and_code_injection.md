# Dynamic Polymorphism with Metaclasses and Code Injection - Sy Brand

## Polymorphism

- The provision of a single interface to entities of different types.
- Two types of way to do polymorphism:

### Dynamic polymorphism

- Run-time
- Different behavior based on dynamic type
- Typically implemented with inheritance

### Static polymorphism

- Compile-time
- Different behavior based on static type
- Typically implemented with overloading and templates

## Problems 1 of inheritance: It often requires dynamic allocation

Consider:

```cpp
struct Base{};
struct A : Base{};
struct B : Base{};

Base make_base() {
    return A{};
}

std::vector<Base> v;
v.push_back(A{});
v.push_back(B{});
```

- Above snippet is going to trigger slicing - as compiler just take the Base part of the object, and slice off the dynamic part of the derived object.
- Usually this is not what we want, and in order to avoid this, we need to do this with dynamic allocate memory:

```cpp
struct Base{};
struct A : Base{};
struct B : Base{};

std::unique_ptr<Base> make_base() {
    return std::make_unique<A>();
}

std::vector<std::unique_ptr<Base>> v;
v.push_back(std::unique_ptr<A>());
v.push_back(std::unique_ptr<B>());
```

- Although techniques build on inheritance that can avoid the dynamic allocation, like small buffer optimization, always use in-store storage ...etc. Still, by default, when you use inheritance, you have to use dynamic allocation so the polymorphism behavior is as expected.

## Problems 2 of inheritance: Ownership and nullability consideration

- Since we have pointer, we have to think about ownership (fine if it's in unique_ptr)
- Also we need to consider if it could be null. And if it's null, what's the expected behavior etc


## Problems 3 of inheritance: Intrusiveness: requires modifying child classes

```cpp
namespace mylib {
struct Base {
  virtual void do_thing();
};
}  // namespace mylib

namespace otherlib {
struct X {
  virtual void do_thing();
};
}  // namespace otherlib

mylib::Base* b = new otherlib::X; // this will not work because otherlib::X
                                  // is not inherited from mylib::Base
```


## Problems 4 of inheritance: No more value semantics

- We have pointers, if we want value semantics, we need to have something like a virtual clone function, which dynamically allocate pointer and passed it back to create the derived type. This is the only way to get the correct copy behavior.
- Doing so, however, you still don't have the usually C++ value semantics, which a lot of code can depend on.

## Problems 5 of inheritance: Changes semantics for algorithms and containers

- For example, say you are doing a `sort`. When you are sorting pointers, you have to supply your own comparator.
- Or maybe, when you store this thing in `set`, you might need to do the same thing.

Overall, from above 5 problems, you can see: once you use inheritance, you are not in the usual C++ values world.

- How to solve this?


## Implementation 1: normal virtual code

```cpp
struct animal {
  virtual ~animal();
  virtual void speak() = 0;
};

struct cat : animal {
  void speak() override;
};

struct dog : animal {
  void speak() override;
};
```

- Say you have a `cat` object. And you want to call `speak`. There are 2 indirections happen. First, compiler need to find the vtable for `cat`, then it needs to find the `cat::speak` function.
- All these indirections could be a performance bottleneck. This is not the interface we want.
- What we want should be something like:

```cpp
struct animal {
  // some magic
};

struct cat {
  void speak();
};

struct dog {
  void speak();
};
```

- And I would like to use above interface to do this without any slicing.

```cpp
animal c = cat{};
c.speak();

animal d = dog{};
d.speak();
```

- What's the trick!?


## The plan: Hand-written virtual function

### Step 1: Declare vtable for the abstract interface

- What should such vtable contains? 2 function pointers to mimic the virtual functions and destructor.
- Taking void* pointers because that's how we are going to store the concrete object internally.

```cpp
struct vtable {
  void (*speak)(void* ptr);
  void (*destroy_)(void* ptr);
};
```

### Step 2: Define vtable for a concrete type

- This should provide the bridge to call the concrete speak and concrete destructor

```cpp
// this is basically create an vtable object and passing in 2 lambda functions
// to the constructor
template <typename Concrete>
constexpr vtable vtable_for{
    // function which calls speak
    [](void* ptr) { static_cast<Concrete*>(ptr)->speak(); },
    // function which deletes object
    [](void* ptr) { delete static_cast<Concrete*>(ptr); }};
```

### Step 3: Capture the vtable pointers on construction

- Check the templated constructor of animal. This technique is basically called type-erasure.
- When you create animal, you passed in the concrete type T. And it's dynamically allocated accordingly.
- Also the vtable is constructed with the bridge function `vtable_for` that can mimic the C++ vtable indirection.

```cpp
struct animal {
  void* concrete_;d
  vtable const* vtable_;

  template <typename T>
  animal(T const& t) : concrete_(new T(t)), vtable_(&vtable_for<T>) {}
};
```

### Step 4: Forward calls through the vtable

```cpp
struct animal {
  // anything defined above plus new 2 functions to bridge to the vtable
  void speak() { vtable_->speak(t_); }
  ~animal() { vtable_->destroy_(t_); }
};
```

- Then we can do:

```cpp
struct cat { void speak(); };
struct dog { void speak(); };

int main() {
  animal c = cat{};
  c.speak();

  animal d = dog{};
  d.speak();
}
```

- We solve the ownership and nullability consideration here, as we create the the concrete type in `animal` ctor.
- We solve the issue of intrusiveness. We can just define `cat` and `dog`, passed in and it's just worked.
- We still need to dynamic allocate memory
- We still don't have direct value semantics

## Solve issue for copy and move

- Through extend our `vtable` struct and `vtable_for` bridge

```cpp
struct vtable {
  // except for the member defined before, add ...
  void* (*clone_)(void* ptr);       // newly add
  void* (*move_clone_)(void* ptr);  // newly add
};

template <typename Concrete>
constexpr vtable vtable_for{
    // except for the definitions defined before, add ...
    [](void* ptr) -> void* { return new T(*static_cast<T*>(ptr)); },
    [](void* ptr) -> void* { return new T(std::move(*static_cast<T*>(ptr))); }};

struct animal {
  // except for the definitions defined before, add ...
  animal(animal const& rhs)
      : t_(rhs.vtable_->clone_(rhs.t_)), vtable_(rhs.vtable_) {}
  animal(animal&& rhs)
      : t_(rhs.vtable_->move_clone_(rhs.t_)), vtable_(rhs.vtable_) {}
  animal& operator=(animal const& rhs) {
    t_ = rhs.vtable_->clone_(rhs.t_);
    vtable_ = rhs.vtable_;
    return *this;
  }
  animal& operator=(animal&& rhs) {
    t_ = rhs.vtable_->move_clone_(rhs.t_);
    vtable_ = rhs.vtable_;
    return *this;
  }
};
```

- And now we can do something like:

```cpp
animal a = cat{};
a.speak;
a = dog{}; // re-assign
a.speak;
animal b = a; // copy ctor without slicing
b.speak;
```

- And we can also do:

```cpp
std::vector<animal> animals { cat{}, dog{} };
for (auto&& a : animals) {
  a.speak();
}
```

- So now, we solve the issue of no value semantics.
- We also solve the issue of "changing semantics for algorithms and containers" - as long as we define the comparator for the concrete type.

## The problem of type erasure

- A lot of (weird) code that could get wrong.
- And we don't want to do this whenever we have a polymorphism needs.
- There could be some magic lib to reduce the boilerplate. But this is not what we aim here.
- We will solve with something that (potentially) could be added to new standard: static reflection.

## What is "reflection"?

- "The ability of a program to introspect its own structure"
- and expose the structure to the rest of the program.
- What is "static reflection"? The ability to do reflection at compile-time!
- We do have some capability to do static reflection in C++ at the moment, most of them are in the `type_traits` header. For examle:
  - `std::is_array<T>`: querying T's state (if it's array) statically, asking questions about your type, is a form of reflection
  - `std::is_same<T, U>`: also querying the state of the program.
  - etc
- But how about convert enum to string, or iterate over members? For example:
  - [Scalable Reflection in C++](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p1240r2.pdf)
- The goal is something like below:

```cpp
template<Enum T>
constexpr std::string to_string(T value) {
  for constexpr (auto e : std::meta::members_of(reflexpr(T))) {
    if (exprid(e) == value) {
      return std::meta::name_of(e);
    }
  }
  return "<unnamed>";
}
```

- `reflexpr`: reflect on the type `T` and returns an object representing the information of `T`
- `std::meta::members_of`: gives us a range representing the members of that type
- `exprid(e)`: changes the reflection information into an actual expression in C++ world

## Code injection: the technique to further improve this

- Say we want to have getter for private members like:

```cpp
class point {
  int x;
  int y;
public:
  int get_x() { return x; }
  int get_y() { return y; }
};
```

- Ideally we want to have something like this instead:


```cpp
class point {
  int x;
  int y;
  consteval {
    generate_getters(reflexpr(point));
  }
};
```

- Ideally, `consteval` specify do this at compile-time, and the `generate_getters` inject some code for us at compile-time accordingly. It should look something like:

```cpp
consteval void generate_getter(std::meta::info cls) {
  for (auto member : std::meta::members_of(cls)) {
    if (std::meta::is_nonstatic_data_member(member)) {
      generate_getter(member);
    }
  }
}
```

- `meta::info`: the representation of the reflected information

```cpp
consteval void generate_getter(std::meta::info member) {
  -> __fragment struct {                    // which inject things like:
    typename(meta::type_of(member)) const&  // int const&
    unqualid("get_", member)() {            // get_x() {
      return exprid(member);                //   return x;
    }                                       // }
  };
}
```

- The magic is the `__fragment`
- `__fragment` is an injection statement (probably won't be the final version in standard, but something to show)
- `__fragment` is going to inject some code into the context that we are currently executing at compile time. We define the context as `struct` here (not `class` for the default data visibility). (The context could be `class`, `struct`, `block` (inject into block of code), `new_space` (?) ...etc

## Back to our goal, improve dynamic polymorphism with code injection

- The goal is to generate the whole boilerplate of type erasure logic using code injection!

```cpp
struct animal {
  void* concrete_;
  vtable const* vtable_;
  // ctors, forwarding functions, etc
};
```

- To make this general, we expect to make something like this:

```cpp
template <typename Facade>
struct typeclass_for {
  void* concrete_;
  vtable<Facade> const* vtable_;
  // ctors, forwarding functions, etc
};
```

- `typeclass_for` is like a general version of `animal` defined above.
- Facade should be a **description interface** for your type
  - The `speak` function should never be defined. It's just a declaration.
  - It's like a blind interface which says "this is what an animal should look like"
  - It's sort of like concept, but instead of define on usage pattern, it's defined on function signature.

```cpp
struct animal_facade {
  void speak();
};

using animal = typeclass_for<animal_facade>;
```

- Code-injected virtual functions should follow the same pattern when we do manual vtable.

### Step 1: Declare vtable for the abstract interface

- In the manual version one, we have:

```cpp
struct vtable {
  void (*speak)(void* ptr);          // specific when it's vtable for animal
  void (*destroy_)(void* ptr);       // common in vtable
  void* (*clone_)(void* ptr);        // common in vtable
  void* (*move_clone_)(void* ptr);   // common in vtable
};
```

- Here, except `speak` is something customized for `animal`'s context, all the other 3 functions are common in vtable.
  - And in `animal` context, we want to generate a function pointer in the vtable for the `speak`.
- With this in mind, in a more generic context, there could be multiple such customized functions.
  - So what we want to do for the generic version is, for any kind of Facade passed into vtable, and for all the functions in the Facade, we want to generate a function pointer in the vtable accordingly.

```cpp
template <typename Facade>
struct vtable {
  void (*speak)(void* ptr);
  //...
}
```

- To do this in code-injection way:
  - `for_each_declared_function` is the helper function to generate all declared functions from reflection of facade, it will calls the lambda in the second parameters accordingly
  - `->params` means inject all the other parameters of the function

```cpp
template <typename Facade>
struct vtable {
  consteval {
    for_each_declared_function(reflexpr(Facade),
      [](auto func, auto ret, auto params) constexpr {
        -> __fragment struct {
            // basically, in animal context: void (*speak)(void* ptr);
            typename(ret) (*unqualid(func))(void* ptr, ->params);
        };
      });
  }
  //...
}
```

### Step 2: Declare vtable for a concrete type

- We need something like this:

```cpp
table.speak = [](void* ptr) {
  static_cast<T*>(ptr)->speak();
};
```

- And it should be done in an injected way:

```cpp
-> __fragment {
  table.speak = [](void* ptr) {
    static_cast<T*>(ptr)->speak();
  };
};
```

- Then make it generic again

```cpp
-> __fragment {
  table.unqualid(func) = [](void* ptr) {
    static_cast<T*>(ptr)->unqualid(func)();
  };
};
```

- Then add additional parameters in the generic context

```cpp
-> __fragment {
  table.unqualid(func) = [](void* ptr, ->params) {
    static_cast<T*>(ptr)->unqualid(func)(unqualid(...params));
  };
};
```

- and we can just add a return if such function is going to return

```cpp
-> __fragment {
  table.unqualid(func) = [](void* ptr, ->params) {
    return static_cast<T*>(ptr)->unqualid(func)(unqualid(...params));
  };
};
```

### Step 3: Capture the vtable pointers on construction

- This part is the same.

```cpp
template<typename Facade>
struct typeclass_for {
  void* concrete_;
  vtable<Facade> const* vtable_;

  template<typename T>
  typeclass_for(T const& t) :
    concrete_(new T(t)),
    vtable_(&vtable_for<Facade, T>)
  {}
};
```

### Step 4: Forward calls through the vtable

- Similar transformation, overall inject the forward calls

```cpp
template<typename Facade>
struct typeclass_for {
  consteval {
    for_each_declared_function(reflexpr(Facade),
      [](auto func, auto ret, auto params) consteval {
      -> __fragment struct {
        typename(ret) unqualid(func) (->params) {
          return this->vtable_->unqualid(func)(
            this->concrete_, unqualid(...params)
          );
        };
      }
    });
  };
};
```

- And finally, we can do

```cpp
struct animal_facade {
  void speak();
};

using animal = typeclass_for<animal_facade>;
```

## So, what about the problem of dynamic allocation, we still haven't solve it?

```cpp
template<typename Facade>
struct typeclass_for {
  void* concrete_;
  vtable<Facade> const* vtable_;
  // ctors, forwarding functions, etc.
};
```

- What we can do, is further template our class here:

```cpp
template<typename Facade, typename StoragePolicy>
struct typeclass_for {
  StoragePolicy storage_;
  vtable<Facade> const* vtable_;
  // ctors, forwarding functions, etc.
};
```

- or even:

```cpp
template<typename Facade, typename StoragePolicy, typename VTablePolicy>
struct typeclass_for {
  StoragePolicy storage_;
  VTablePolicy::vtable<Facade> const* vtable_;
  // ctors, forwarding functions, etc.
};
```

## Can we even make this more readable?

- Imagine we can do some "metaclass"

```cpp
class(typeclass) animal {
  void speak();
};

// where it's the same as defined
// struct animal_facade {
//   void speak();
// }
// using animal = typeclass_for<animal_facade>;
```

## What is metaclass?

- Definition from the [paper](https://wg21.link/P0707)
- Metaclass functions let programers write a new kind of efficient abstraction: a user-defined named subset of classes that share common characteristics, typically (but not limited to):
  - User-defined rules
  - defaults, and
  - generated functions

- In the polymorphism's case, we mainly want to utilize the "generated functions" as common characteristics.
- How to do it? For example, say we have a simple `point` class where it's essentially `std::pair` with names

```cpp
class point {
  int x;
  int y;
};
```

- A metaclass of point might look like:

```cpp
class(value) point {
  int x;
  int y;
};
```

- The metaclass `class(value)` will basically help you to create the point class in a value seantic way, where several common functions might be defined automatically. (Like comparator, getter, setter ...etc)

- Overall, this should be the same as if we ...

```cpp
namespace __hidden {
  struct prototype {
    int x; // took from class(value) body
    int y; // took from class(value) body
  };
}

struct point {
  using prototype = __hidden::prototype;
  consteval {
    value(reflexpr(prototype));
  }
}
```

- Where `value` might look like ...

```cpp
consteval void value(std::meta::info source) {
  // injection statements
}
```

- Similarly, we might have `typeclass` that look like

```cpp
consteval void typeclass(std::meta::info source) {
  // injection statements
}
```

- And we might have

```cpp
class(typeclass) animal {
  void speak();
};

std::vector<animal> animals;
```

## Links

- [code in this talk](https://godbolt.org/z/bUDyhG)
  - [backup src](code_inject_polymorphism.h)
- [typeclasses prototype implementation](https://github.com/TartanLlama/typeclasses)
