# smart_pointer

Taking notes from

> - [fluent C++](https://www.fluentcpp.com/2018/12/25/free-ebook-smart-pointers/)
> - [acodersjourney.com post](https://www.acodersjourney.com/top-10-dumb-mistakes-avoid-c-11-smart-pointers/)

<details><summary markdown="span">Intros</summary>

> :brain: The programming task of ensuring that all objects are correctly deleted is very low in terms of
levels of abstraction, and since **writing good code essentially comes down to respecting levels of
abstraction**, you want to keep those tasks away from your business logic (or any sort of logic for
that matter)


C++ has several types of memories, that correspond to different parts of the physical memory.

- They are: the **static**, the **stack**, and the **heap**.


**The stack**

Objects allocated on the stack are automatically destroyed when they go out of scope.

- In C++ a scope is defined by a pair of brackets (​{​ and ​}​) except those used to initialize objects

There are 3 ways for an object to go out of scope:

- encountering the next closing bracket (}),
- encountering a return statement,
- having an exception thrown inside the current scope that is not caught inside the current scope.


**The heap**

- The heap is where dynamically allocated objects are stored, that is to say ​objects that are allocated with a call to `new​`, which returns a pointer
- strictly speaking, the memory allocated by new is called the **free store**.
- The heap is the memory allocated by `​malloc​`, `​calloc` and `​realloc`
- The term 'heap' is so ubiquitous in developer jargon to talk about any dynamically allocated memory that I am using it here in that sense.
- Anyway to destroy an object allocated by new, we have to do it manually by calling `delete`
- Contrary to the stack, **objects allocated on the heap are ​not destroyed automatically​.**
  - This offers the **advantages of keeping them longer than the end of a scope**, and without incurring any copy except those of pointers which are very cheap.
- But as a price to pay for this flexibility it puts you, the developer, in charge of their deletion.

> :brain: `delete` has to be called ​**once and only once** to deallocate a heap-based object.

- If it is not called the object is not deallocated, and its memory space is not reusable - this is called a memory leak.
- But on the other hand, a delete called **more than once on the same address leads to undefined behaviour**.

**RAII**

- The principle of RAII is simple: wrap a resource (a pointer for instance) into an object, and
dispose of the resource in its destructor.

```cpp
template <typename T>
class SmartPointer
{
public:
    explicit SmartPointer(T* p) : p_(p) {}
    ~SmartPointer() { delete p_; }
private:
    T* p_;
};
```

- The point is that **you can manipulate smart pointers as objects allocated on the stack.**
- And the compiler will take care of automatically calling the destructor of the smart pointer because...
**objects allocated on the stack are automatically destroyed when they go out of scope​.**
- And this will therefore call delete on the wrapped pointer. Only once.
- The problem above is ... what if you do:

```cpp
{
    SmartPointer<int> sp1(new int(42));
    SmartPointer<int> sp2 = sp1; // now both sp1 and sp2 point to
                                 // the same object
} // sp1 and sp2 are both destroyed, the pointer is deleted twice!
```

- it deletes the underlying object twice, leading to undefined behaviour. How to deal with copy then? This is a feature on which the various types of smart pointer differ.


## Raw pointers

- Raw pointers share a lot with references, but the latter should be preferred except in some cases
- **Raw pointers and references represent access to an object, but not ownership​.**
- When you hold an object with a `​unique_ptr` and want to pass it to an interface. You don’t pass the `​unique_ptr​`, nor a reference to it, but rather **a reference to the pointed to object:**

```cpp

void renderHouse(const House& house)
//...
std::unique_ptr<House> house = buildAHouse();
renderHouse(*house);
```

</details>


## `std::unique_ptr`

- The semantics of ​std::unique_ptr is that it is the **sole owner** of a memory resource.
- It tells you that it gives you a pointer to a house, of which you are the owner. ​No one else will delete this pointer except the ​unique_ptr that is returned by the function.
- Note though that even when you receive a `unique_ptr`, you're not guaranteed that no one else has access to this pointer. Indeed, if another context keeps a copy of the pointer inside your unique_ptr​, then modifying the pointed to object through the `​unique_ptr` object will of course impact this other context. If you don't want this to happen, the way to express it is by using a `​unique_ptr​` to `​const​`:

```cpp
std::unique_ptr<const House> buildAHouse();
// for some reason, I don't want you
// to modify the house you're being passed
```

`std::unique_ptr` **cannot be copied.**

- To ensure that there is only one unique_ptr that owns a memory resource.
- The ownership can however be transferred from one ​unique_ptr to another (which is how you can pass them or return them from a function) by moving a `​unique_ptr` into another one.

```cpp
std::unique_ptr<int> p1 = std::make_unique(42);
std::unique_ptr<int> p2 = move(p1); // now p2 hold the resource
                                    // and p1 no longer hold anything
```

> :rotating_light: `unique_ptr` does what `auto_ptr` was intended to do. auto_ptr use copy constructor to transfer the ownership, but leave the original pointer in a bad state without explicit non-copyable protection like `unique_ptr` provides.

> :bulb: when we don't use custom deleters, we shouldn't use new directly, but prefer `std::make_unique` that lets you pass the arguments for the construction of the pointed-to obj (Item 17 of Effective C++)


## `std::shared_ptr`

- A single memory resource can be held by several `​std::shared_ptr​s` at the same time​.
- The `​shared_ptr​s` internally maintain a count of how many of them there are holding the same resource, and when the last one is destroyed, it deletes the memory resource.
- Therefore `​std::shared_ptr` allows copies, but with a reference-counting mechanism to make sure that every resource is deleted once and only once.

> :brain: `std::shared_ptr`​ should not be used by default

- Having several simultaneous holders of a resource makes for a **​more complex** system than with one unique holder, like with `​std::unique_ptr`​.
- Having several simultaneous holders of a resource makes **​thread-safety​ harder**,
- It makes the **​code counter-intuitive** when an object is not shared in terms of the domain and still appears as "shared" in the code for a technical reason,
- It can incur a **​performance cost, both in time and memory**, because of the bookkeeping related to the reference-counting.
- A `shared_ptr` needs to maintain the thread-safe refcount of objects it points to and a control block under the covers which makes it more heavyweight than an `unique_ptr`.

> :brain: Recommendation – By default, you should use a `unique_ptr`. If a requirement comes up later to share the resource ownership, you can always change it to a shared_ptr.

> :rotating_light: An easy mistake of using `​std::shared_ptr` is **not making resources/objects shared by `shared_ptr` thread-safe**

- shared_ptr allows you to share the resource thorough multiple pointers which can essentially be used from multiple threads. It’s a common mistake to assume that wrapping an object up in a `shared_ptr` makes it inherently thread safe. **It’s still your responsibility to put synchronization primitives around the shared resource managed by a `shared_ptr`.**

> :brain: Use `make_shared` have performance gain!

- If without, there will be 2 allocations: one for the object itself from the new, and then a second for the manager object created by the shared_ptr constructor. If with `make_shared`, C++ compiler does a single memory allocation big enough to hold both the manager object and the new object.

> :brain: If you’re not using `make_shared` to create the `shared_ptr`, at least assign the raw pointer to a `shared_ptr` as soon as it is created:

- `std::shared_ptr ptr(new SomeObj());`
- As the raw pointer should never be used by another `shared_ptr(T*)` constructor again, it would have create duplicated reference count/control block ...etc, which eventually cause double deletion!


## `std::weak_ptr`

- `std::weak_ptr​s` can hold a reference to a shared object along with other `std::shared_ptr`​s, but **they don't increment the reference count.**
- This means that if no more `std::shared_ptr` are holding an object, this object will be deleted even if some weak pointers still point to it.
- For this reason, a weak pointer needs to check if the object it points to is still alive: Before you can use a `weak_ptr`, you need to acquire the `weak_ptr` by calling a `weak_ptr::lock()`
  - The `lock()` method **essentially upgrades the `weak_ptr` to a `shared_ptr` such that you can use it.**
  - However, if the `shared_ptr` object that the `weak_ptr` points to is no longer valid, the `weak_ptr` is emptied. Calling any method on an expired `weak_ptr` will cause an access violation.
  - So, to do this, the interface is basically copied the `weak_ptr` into to a `​std::shared_ptr`**​:

```cpp
void useMyWeakPointer(std::weak_ptr<int> wp) {
    if (std::shared_ptr<int> sp = wp.lock()) {
        // the resource is still here and can be used
    } else {
        // the resource is no longer here
    }
}
```

A typical use case for this is about **​breaking `​shared_ptr` circular references**​.

```cpp
struct House { std::shared_ptr<House> neighbor; };
std::shared_ptr<House> house1 = std::make_shared<House>();
std::shared_ptr<House> house2 = std::make_shared<House>();;
house1->neighbor = house2;
house2->neighbor = house1;
```

- None of the houses ends up being destroyed at the end of this code, because the `​shared_ptr​s` points into one another. But if one is a `​weak_ptr` instead, there is no longer a circular reference.
- Another use case is that `​weak_ptr` can be used to **maintain a cache**​: The data may or may not have been cleared from the cache, and the `weak_ptr​` references this data.

## `scoped_ptr`

- It simply **disables the copy and even the move construction**.
- So it is the **sole owner of a resource, and its ownership cannot be transferred.**
- Therefore, **a `​scoped_ptr` can only live inside a scope. Or as a data member of an object.**
- And of course, as a smart pointer, it keeps the advantage of deleting its underlying pointer in its destructor.


## Case study: Transferring sets of `unique_ptr`s

- Imagine we have ​a collection of `std::unique_ptr<Base>​`s, and we want to use it in a polymorphism way.
- the comparison between elements of the set will call the `​operator<` of `​std::unique_ptr`​, which compares the memory addresses of the pointers inside them.
- In most cases, this is not what you want. When we think "no duplicates", it generally means "no logical duplicates" as in: no two elements have the same value. And **not "no two elements are located at the same address in memory"**
- To implement no logical duplicates, we need to call the ​operator< on Base (provided that it exists, maybe using an id provided by ​Base for instance) to compare elements and determines whether they are duplicates. And to make the set use this operator, we need to customize the comparator of the set:

```cpp
struct ComparePointee
{
template<typename T>
bool operator()(std::unique_ptr<T> const& up1, std::unique_ptr<T> const& up2) {
    return *up1 < *up2;
}
};

template<typename T>
using UniquePointerSet = std::set<std::unique_ptr<T>, ComparePointee>;

UniquePointerSet mySet;
```

But then ... can we actually do the following?

```cpp
UniquePointerSet<Base> source;
source.insert(std::make_unique<Derived1>());
source.insert(std::make_unique<Derived2>());
source.insert(std::make_unique<Derived3>());

//...

UniquePointerSet<Base> destination;
// To ​transfer elements efficiently​, we use the insert method:
destination.insert(begin(source), end(source));
```

We actually can't! There will be compile error about the deleted copy ctor being called! But WHY??? Because the insert methods attemps to make a copy of the ​unique_ptr​ elements. What to do then? C++17 adds the `std::set::merge` for this

```cpp
destination.merge(source);
```

- This makes destination ​take over ​the nodes of the tree inside of source. It's like performing a splicing on lists. So after executing this line, destination has the elements that source had, and source is empty.
- And since it's only the nodes that get modified, and not what's inside them, the ​unique_ptr​s don't feel a thing. They are not even moved. destination​ now has the ​unique_ptr​s.

There will be some hassel to do it pre C++17, the issue is that `set` provides access to its elements. When dereferenced, a set's iterator does not return a `​unique_ptr`&​, but rather a `​const ​unique_ptr&`​.

- It is to make sure that the values inside of the set don't get modified without the set being aware of it. Indeed, it could break its invariant of being sorted.

:bulb: So even if you do `std::move(begin(source), end(source), std::inserter(destination, end(destination)));`, you will still hit the "deleted copy ctor being used" compilation error, why?

- `std::move`​ dereferences the iterator on `set` and gets a `​const unique_ptr&​`
- it calls `​std::move`​ on that references, thus getting a ​`const unique_ptr&&​`,
- it calls the `​insert` method on the insert output iterator and passes it this `const unique_ptr&&`​,
- the `​insert` method has two overloads: one that takes a `​const unique_ptr&`​, and one that takes a `​unique_ptr&&`​.
- Because of the const in the type we're passing, the compiler cannot resolve this call to the second method, and calls the first one instead, which eventually make a copy!!!

Solution 1: Give up on the move and accepting to copy the elements from a set to another, we need to make a copy of the contents pointed by the `​unique_ptr​s`.

```cpp
class Base {
 public:
  virtual std::unique_ptr<Base> cloneBase() const = 0;
  //...
};

class Derived : public Base {
 public:
  std::unique_ptr<Base> cloneBase() const override {
    return std::make_unique<Derived>(*this);
  }
  //...
};

auto clone = [](std::unique_ptr<Base> const& pointer) {
  return pointer->cloneBase();
};

std::transform(begin(source), end(source),
               std::inserter(destination, end(destination)), clone);
// or with for loop
/*
for (auto const& pointer : source) {
  destination.insert(pointer->cloneBase());
}
*/
```

Solution 2: Keeping the move and throwing away the set

- The set that doesn't let the move happen is the source set. If you only need the destination to have unique elements, you can replace the source set by a `​std::vector`​.
- `​std::vector` does not add a ​const to the value returned by its iterator. We can therefore move its elements from it with the ​`std::move`​ algorithm:

```cpp
std::vector<std::unique_ptr<Base>> source;
source.push_back(std::make_unique<Derived>(42));
std::set<std::unique_ptr<Base>> destination;
std::move(begin(source), end(source),
          std::inserter(destination, end(destination)));
```

- Then the destination set contains a ​unique_ptr that has the contents that used to be in the one of the source, and the source vector now contains an empty `​unique_ptr​`.

## Case study: `unique_ptr`'s custom delete interface

Say you have a type with 2 custom deleter:

```cpp
using ComputerConstPtr = std::unique_ptr<const Computer, void(*)(const Computer*)>;
void deleteComputer(const Computer* computer){ delete computer;}
void skipDeleteComputer(const Computer* computer) {}

// ...

ComputerConstPtr myComputer(new Computer, deleteComputer);
ComputerConstPtr yourComputer(new Computer, skipDeleteComputer);
```

- You can see where ever you want to specify such pointer, you need to pass in the custom function. This is very verbose

How about templatized deleter? Not really making things better.

```cpp
template <typename T>
void doDelete(const T* p) {
  delete p;
}

template <typename T>
void skipDelete(const T* x) {}

ComputerConstPtr myComputer(new Computer, doDelete<Computer>);
ComputerConstPtr yourComputer(new Computer, skipDelete<Computer>);
```

So the recommended way:

```cpp
namespace util {

template <typename T>
void doDelete(const T* p) {
  delete p;
}

template <typename T>
void skipDelete(const T* x) {}

template <typename T>
using CustomUniquePtr = std::unique_ptr<const T, void (*)(const T*)>;

template <typename T>
auto MakeConstUnique(T* pointer) {
  return CustomUniquePtr<T>(pointer, doDelete<T>);
}
template <typename T>
auto MakeConstUniqueNoDelete(T* pointer) {
  return CustomUniquePtr<T>(pointer, skipDelete<T>);
}

}  // namespace util
```

Then you can do below, no longer need to keep passing in the custom deleter:

```cpp
auto myComputer = util::MakeConstUnique<>(new Computer);
auto yourComputer = util::MakeConstUniqueNoDelete<>(new Computer);
```

## Case study: covariance return type By Raoul Borges

- [The issue](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Covariant_Return_Types):

- Normally Base class defines the interface. So here, for the `Derived::clone` function, it seems like we have to return `Base*` as well.
- The result is that if we want to use, we have to do a `dynamic_cast`:

```cpp
class Base {
 public:
  virtual Base* clone() const { return new Base(*this); }
};
class Derived : public Base {
 public:
  virtual Base* clone() const override { return new Derived(*this); }
};

//...
Derived* d1 = new Derived();
Base* b = d1->clone();
Derived* d2 = dynamic_cast<Derived*>(b);
if (d2) {
  // Use d2 here.
}
```

C++ actually supports "covariance return type", where we can actually do:

```cpp
class Base {
 public:
  virtual Base* clone() const { return new Base(*this); }
};
class Derived : public Base {
 public:
  // No longer return Base*, this alternative is direct and saves unnecessary casts.
  virtual Derived* clone() const override { return new Derived(*this); }
};

//...
Derived* d1 = new Derived();
Derived* d2 = d1->clone();
if (d2) {
  // Use d2 here.
}
```

However, this doesn't apply to smart pointer directly, though:

```cpp
#include <memory>
struct Base {};
struct Derived : Base {};

struct Parent {
  virtual std::unique_ptr<Base> foo();
};

struct Child : Parent {
  //  compiler generates an error.
  virtual std::unique_ptr<Derived> foo() override;
};
```

Resolution 1: through separation of concerns + private virtual function

```cpp
class some_class {
 public:
  std::unique_ptr<some_class> clone() const {
    // transfers the ownership of the pointer to the newly created object.
    // While this is usually unsafe, we assume that in this case no one can call
    // this function except the ​clone() function, which is enforced by
    // the private access of ​clone_impl()​.
    return std::unique_ptr<some_class>(this->clone_impl());
  }

 private:
  // separation of concerns + private virtual function
  // One could get uneasy at the idea of having a clone_impl member function
  // using a RAII-unsafe transfer of ownership, but the problem is mitigated as
  // the member function is private, and is called only by clone. This limits
  // the risk as the user of the class can’t call it by mistake
  virtual some_class* clone_impl() const { return new some_class(*this); }
};
```

Then we can do:

```cpp
class cloneable {
 public:
  virtual ~cloneable() {}
  std::unique_ptr<cloneable> clone() const {
    return std::unique_ptr<cloneable>(this->clone_impl());
  }

 private:
  virtual cloneable* clone_impl() const = 0;
};

class concrete : public cloneable {
 public:
  // [trick] C++ features: name hiding (i.e. when declaring a name in a derived
  // class, this name hides all the symbols with the same name in the base class
  // ), we hide (not override) the ​clone() member function to return a smart
  // pointer of the exact type we wanted!
  std::unique_ptr<concrete> clone() const {
    return std::unique_ptr<concrete>(this->clone_impl());
  }

 private:
  // [trick] you can still use return covariance through separation of concern!!
  virtual concrete* clone_impl() const override { return new concrete(*this); }
};

int main() {
  // When cloning from a concrete, we obtain a ​unique_ptr<concrete>​,
  std::unique_ptr<concrete> c = std::make_unique<concrete>();
  std::unique_ptr<concrete> cc = c->clone();
  // When cloning from a cloneable, we obtain a ​unique_ptr<cloneable>​.
  cloneable* p = c.get();
  std::unique_ptr<clonable> pp = p->clone();
}
```

**Improve further: use CRTP**

`clone_inherit` is a CRTP that knows its derived class, but also all its direct base class.
- It implements the covariant `​clone_impl()` and hiding `​clone()` member functions as usual, but
they use casts to move through the hierarchy of types.
- This effectively adds a polymorphic and covariant ​clone()​ to a hierarchy of class

```cpp
template <typename Derived, typename Base>
class clone_inherit<Derived, Base> : public Base {
  public : std::unique_ptr<Derived> clone() const {
    return std::unique_ptr<Derived>(static_cast<Derived*>(this->clone_impl()));
  }

 private:
  virtual clone_inherit* clone_impl() const override {
    return new Derived(*this);
  }
};

class concrete : public clone_inherit<concrete, cloneable> {};

int main() {
  std::unique_ptr<concrete> c = std::make_unique<concrete>();
  std::unique_ptr<concrete> cc = b->clone();
  cloneable* p = c.get();
  std::unique_ptr<clonable> pp = p->clone();
}
```

**Problem: Multiple Inheritance - Variadic templates to the rescue**

how can we extend our solution to support the case where the concrete class inherits from two bases classes that both provide the same clone feature?

- The solution first needs the two base classes, ​foo and ​bar​, to offer the ​clone​/​clone_impl member functions:

```cpp
class foo {
 public:
  virtual ~foo() = default;
  std::unique_ptr<foo> clone() const {
    return std::unique_ptr<foo>(this->clone_impl());
  }

 private:
  virtual foo* clone_impl() const = 0;
};

class bar {
 public:
  virtual ~bar() = default;
  std::unique_ptr<bar> clone() const {
    return std::unique_ptr<bar>(this->clone_impl());
  }

 private:
  virtual bar* clone_impl() const = 0;
};
```

- We only need to modify the ​clone_inherit​ CRTP with variadic templates to support it:

```cpp
template <typename Derived, typename... Bases>
class clone_inherit : public Bases... {
 public:
  std::unique_ptr<Derived> clone() const {
    return std::unique_ptr<Derived>(static_cast<Derived*>(this->clone_impl()));
  }

 private:
  virtual clone_inherit* clone_impl() const override {
    return new Derived(static_cast<const Derived&>(*this));
  }
};

// We can now write our concrete class using:

class concrete : public clone_inherit<concrete, foo, bar>
{};

// we can use our classes with both covariance and smart pointers:
int main() {
  std::unique_ptr<concrete> c = std::make_unique<concrete>();
  std::unique_ptr<concrete> cc = c->clone();
  foo* f = c.get();
  std ::unique_ptr<foo> ff = f->clone();
  bar* b = c.get();
  std::unique_ptr<bar> bb = b->clone();
}
```

**Make use of general template in case that doesn't need a base**

- The foo, bar itself has similar boilerplate of clone and clone_impl like the general template clone_inherit, can we make use of it to reduce the boilerplate of foo, bar?
- Yes we can! Through adding specialization of the template above

```cpp
template <typename Derived>
class clone_inherit<Derived> {
 public:
  virtual ~clone_inherit() = default;
  std::unique_ptr<Derived> clone() const {
    return std::unique_ptr<Derived>(static_cast<Derived*>(this->clone_impl()));
  }

 private:
  virtual clone_inherit* clone_impl() const = 0;
};

// This way, we can now re write foo/bar:
class foo : public clone_inherit<foo> {};
class bar : public clone_inherit<bar> {};
class concrete : public clone_inherit<concrete, foo, bar> {};

// and use it:
int main() {
  std::unique_ptr<concrete> c = std::make_unique<concrete>();
  std::unique_ptr<concrete> cc = c->clone();
  foo* f = c.get();
  std ::unique_ptr<foo> ff = f->clone();
  bar* b = c.get();
  std::unique_ptr<bar> bb = b->clone();
}
```

**One other dimension of OO complexity: Deep Hierarchy: Abstracting**

Another complication of OO Hierarchies is that they can go deeper than two levels:

```text
clonable <- abstract <- concrete
```

- non-leaf classes are not supposed to be instantiable by themselves (More Effective C++, item 33).
- In our case, the ​clone_impl​ method in the non-leaf class must then be pure virtual
- To make the template general enough,we need to support the choice of declaring `​clone_impl` pure virtual, or
implemented. How?

First, we add a dedicated type who will be used to “mark” a type:

```cpp
template <typename T>
class abstract_method {};
```

Then, we partially specialize the `clone_inherit` class again to use that type

```cpp

// general: inheritance + clone_impl implemented
template <typename Derived, typename... Bases>
class clone_inherit : public Bases... {
 public:
  virtual ~clone_inherit() = default;

  std::unique_ptr<Derived> clone() const {
    return std::unique_ptr<Derived>(static_cast<Derived*>(this->clone_impl()));
  }

 private:
  virtual clone_inherit* clone_impl() const override {
    return new Derived(static_cast<const Derived&>(*this));
  }
};

// specialization: inheritance + clone_impl NOT implemented
template <typename Derived, typename... Bases>
class clone_inherit<abstract_method<Derived>, Bases...> : public Bases... {
 public:
  virtual ~clone_inherit() = default;
  std::unique_ptr<Derived> clone() const {
    return std::unique_ptr<Derived>(static_cast<Derived*>(this->clone_impl()));
  }

 private:
  virtual clone_inherit* clone_impl() const = 0;
};

// specialization: NO inheritance + clone_impl implemented
template <typename Derived>
class clone_inherit<Derived> {
 public:
  virtual ~clone_inherit() = default;
  std::unique_ptr<Derived> clone() const {
    return std::unique_ptr<Derived>(static_cast<Derived*>(this->clone_impl()));
  }

 private:
  virtual clone_inherit* clone_impl() const override {
    return new Derived(static_cast<const Derived&>(*this));
  }
};

// specialization: NO inheritance + clone_impl NOT implemented
template <typename Derived>
class clone_inherit<abstract_method<Derived>> {
 public:
  virtual ~clone_inherit() = default;
  std::unique_ptr<Derived> clone() const {
    return std::unique_ptr<Derived>(static_cast<Derived*>(this->clone_impl()));
  }

 private:
  virtual clone_inherit* clone_impl() const = 0;
};

// Then we can do

class cloneable : public clone_inherit<abstract_method<cloneable>> {};
class abstracted : public clone_inherit<abstract_method<abstracted>, cloneable> {};

class concrete : public clone_inherit<concrete, abstracted> {};
int main() {
  std::unique_ptr<concrete> c = std::make_unique<concrete>();
  std::unique_ptr<concrete> cc = c->clone();
  abstracted* a = c.get();
  std::unique_ptr<abstracted> aa = a->clone();
  cloneable* p = c.get();
  std::unique_ptr<clonable> pp = p->clone();
}
```

**Final complication ... Diamond Inheritance: Virtual-ing**

```txt
    clonable
    /       \
   foo      bar
    \        /
     concrete
```

- we have a choice to do: Is the base class inherited virtually, or not?
- This choice must thus be provided by `clone_inherit`. The thing is, declaring a virtual inheritance
is much more tricky because of the template parameter pack... Or is it?

```cpp
template <typename T>
class virtual_inherit_from : virtual public T {
  using T::T;
};
```

- This class actually applies the virtual inheritance to its base class ​T​, which is exactly what we wanted. Now, all we need is to use this class to explicit our virtual inheritance need:

```cpp
class foo : public clone_inherit<abstract_method<foo>,
                                 virtual_inherit_from<cloneable>> {};
class bar : public clone_inherit<abstract_method<bar>,
                                 virtual_inherit_from<cloneable>> {};
class concrete : public clone_inherit<concrete, foo, bar> {};
int main() {
  std::unique_ptr<concrete> c = std::make_unique<concrete>();
  std::unique_ptr<concrete> cc = c->clone();
  foo* f = c.get();
  std::unique_ptr<foo> ff = c->clone();
  bar* b = c.get();
  std::unique_ptr<bar> bb = c->clone();
  cloneable* p = c.get();
  std::unique_ptr<cloneable> pp = p->clone();
}
```

**:exploding_head: The whole package**

- This set of techniques shows how, by using a clever but all-in-all simple combo of two orthogonal C++ paradigms, object oriented and generic (templates), we can factor out code to produce results with a concision that would have been difficult or impossible to get in other C-like languages.

- It also shows a list of techniques (simulated covariance, inheritance indirection providing features) that can be applied elsewhere, each relying on C++ features assembled like lego pieces to produce the desired result.

```cpp

template <typename T>
class abstract_method {};

template <typename T>
class virtual_inherit_from : virtual public T {
  using T::T;
};

template <typename Derived, typename... Bases>
class clone_inherit : public Bases... {
 public:
  virtual ~clone_inherit() = default;
  std::unique_ptr<Derived> clone() const {
    return std::unique_ptr<Derived>(static_cast<Derived *>(this->clone_impl()));
  }

 protected:
  // desirable, but impossible in C++17
  // see: http://cplusplus.github.io/EWG/ewg-active.html#102
  // using typename... Bases::Bases;
 private:
  virtual clone_inherit *clone_impl() const override {
    return new Derived(static_cast<const Derived &>(*this));
  }
};

template <typename Derived, typename... Bases>
class clone_inherit<abstract_method<Derived>, Bases...> : public Bases... {
 public:
  virtual ~clone_inherit() = default;
  std::unique_ptr<Derived> clone() const {
    return std::unique_ptr<Derived>(static_cast<Derived *>(this->clone_impl()));
  }

 protected:
  // desirable, but impossible in C++17
  // see: http://cplusplus.github.io/EWG/ewg-active.html#102
  // using typename... Bases::Bases;
 private:
  virtual clone_inherit *clone_impl() const = 0;
};

template <typename Derived>
class clone_inherit<Derived> {
 public:
  virtual ~clone_inherit() = default;
  std::unique_ptr<Derived> clone() const {
    return std::unique_ptr<Derived>(static_cast<Derived *>(this->clone_impl()));
  }

 private:
  virtual clone_inherit *clone_impl() const override {
    return new Derived(static_cast<const Derived &>(*this));
  }
};

template <typename Derived>
class clone_inherit<abstract_method<Derived>> {
 public:
  virtual ~clone_inherit() = default;
  std::unique_ptr<Derived> clone() const {
    return std::unique_ptr<Derived>(static_cast<Derived *>(this->clone_impl()));
  }

 private:
  virtual clone_inherit *clone_impl() const = 0;
};
```

And the user code:

```cpp
class cloneable : public clone_inherit<abstract_method<cloneable>> {};
class foo : public clone_inherit<abstract_method<foo>,
                                 virtual_inherit_from<cloneable>> {};
class bar : public clone_inherit<abstract_method<bar>,
                                 virtual_inherit_from<cloneable>> {};
class concrete : public clone_inherit<concrete, foo, bar> {};
```