# About destruct(or)

## [delete nullptr is fine, delete deleted object is UB!!]()

```cpp
Foo* foo = 0; // Sets the pointer to 0 (C++ NULL)
delete foo; // Won't do anything

// v.s....

Foo* foo = new Foo();
delete foo; // Deletes the object
delete foo; // Undefined behavior
```

- Setting a pointer to 0 (which is "null" in standard C++, the NULL define from C is somewhat different) avoids crashes on double deletes.

## [Why doesn't delete zero out its operand?](https://www.stroustrup.com/bs_faq2.html#delete-zero)

Consider

```cpp
delete p;
// ...
delete p;
```

- If the ... part doesn't touch p then the second "delete p;" is a serious error that a C++ implementation cannot effectively protect itself against (without unusual precautions).
- Since deleting a zero pointer is harmless by definition, a simple solution would be for "delete p;" to do a "p=0;" after it has done whatever else is required. However, C++ doesn't guarantee that.

One reason is that the **operand of delete need not be an lvalue.** Consider:

```cpp
delete p+1;
delete f(x);
```

- Here, the implementation of delete does not have a pointer to which it can assign zero.
- These examples may be rare, but they do imply that it is **not possible to guarantee that any pointer to a deleted object is 0.**
- A simpler way of bypassing that ``rule'' is to have two pointers to an object:

```cpp
T* p = new T;
T* q = p;
delete p;
delete q;	// ouch!
```

- C++ explicitly allows an implementation of delete to zero out an lvalue operand, and I had hoped that implementations would do that, but **that idea doesn't seem to have become popular with implementers.**
- If you consider zeroing out pointers important, consider using a destroy function:

```cpp
template<class T>
inline void destroy(T*& p) { delete p; p = 0; }
```

- Consider this yet-another reason to minimize explicit use of `new` and `delete` by relying on standard library containers, handles, etc.
- Note that passing the pointer as a reference (to allow the pointer to be zero'd out) has the added benefit of preventing `destroy()` from being called for an rvalue:

```cpp
int* f();
int* p;
// ...
destroy(f());	// error: trying to pass an rvalue by non-const reference
destroy(p+1);	// error: trying to pass an rvalue by non-const reference
```

## destructor visibility, using pimpl idiom as example

- from: [fluent C++](https://www.fluentcpp.com/2018/12/25/free-ebook-smart-pointers/)

```cpp
//  Fridge.h
#include <memory>
class Fridge {
 public:
  Fridge();
  void coolDown();

 private:
  class FridgeImpl;
  std::unique_ptr<FridgeImpl> impl_;
};
```

```cpp
//  Fridge.cpp
#include "Engine.h"
#include "Fridge.h"
class FridgeImpl {
 public:
  void coolDown() { /* ... */ }

 private:
  Engine engine_;
};
Fridge::Fridge() : impl_(new FridgeImpl) {}
```

- This fails to compile: `use of undefined type 'FridgeImpl' can't delete an incomplete type`
- The problem is: deleting a pointer leads to undefined behaviour if the type pointed to is incomplete. (For example, FridgeImpl is only forward declared in the header.)
- `std::unique_ptr` happens to check in its destructor if the definition of the type is visible before calling delete. So it refuses to compile and to call delete if the type is only forward declared.
- Since we didn't add the declaration of the destructor in the `Fridge` class, the compiler took over and defined it for us. But compiler-generated methods are declared ​inline​, so they are implemented in the header file directly. And there, the type of ​FridgeImpl is incomplete. Hence the error.

Fix: declare the destructor and thus prevent the compiler from doing it for
us.

```cpp
//  Fridge.h
#include <memory>
class Fridge {
 public:
  Fridge();
  ~Fridge(); //!!!!!!! <----------FIX for above!
  void coolDown();

 private:
  class FridgeImpl;
  std::unique_ptr<FridgeImpl> impl_;
};
```

```cpp
//  Fridge.cpp
#include "Engine.h"
#include "Fridge.h"
class FridgeImpl {
 public:
  void coolDown() { /* ... */ }

 private:
  Engine engine_;
};
Fridge::Fridge() : impl_(new FridgeImpl) {}
Fridge::~Fridge() = default; //!!!!!!! <----------FIX for above!
```
