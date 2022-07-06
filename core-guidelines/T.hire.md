# T.temp-hier: Template and hierarchy rules:
- **Templates are the backbone of C++'s support for generic programming** and **class hierarchies the backbone of its support for object-oriented programming**. 
- The two language mechanisms can be used effectively in combination, but a few design pitfalls must be avoided.

- [T.temp-hier: Template and hierarchy rules:](#ttemp-hier-template-and-hierarchy-rules)
  - [T.80: Do not naively templatize a class hierarchy](#t80-do-not-naively-templatize-a-class-hierarchy)
  - [T.81: Do not mix hierarchies and arrays](#t81-do-not-mix-hierarchies-and-arrays)
  - [T.82: Linearize a hierarchy when virtual functions are undesirable](#t82-linearize-a-hierarchy-when-virtual-functions-are-undesirable)
  - [T.83: Do not declare a member function template `virtual`](#t83-do-not-declare-a-member-function-template-virtual)
  - [T.84: Use a non-template core implementation to provide an ABI-stable interface](#t84-use-a-non-template-core-implementation-to-provide-an-abi-stable-interface)

## T.80: Do not naively templatize a class hierarchy
- Templating a class hierarchy that has many functions, especially many virtual functions, can lead to code bloat.

```cpp
template <typename T> struct Container { // an interface
    virtual T* get(int i);
    virtual T* first();
    virtual T* next();
    virtual void sort();
};

template <typename T> class Vector : public Container<T> {
  public:
    // ...
};

Vector<int> vi;
Vector<string> vs;
```
- It is probably a bad idea to define a sort as a member function of a container, but it is not unheard of and it makes a good example of what not to do.
- Given this, the compiler cannot know if `vector<int>::sort()` is called, so it must generate code for it.
- Similar for `vector<string>::sort()`. Unless those two functions are called that's code bloat.
- Imagine what this would do to a class hierarchy with dozens of member functions and dozens of derived classes with many instantiations.
- Note: In many cases you can **provide a stable interface by not parameterizing a base**; see "stable base" and OO and GP


## T.81: Do not mix hierarchies and arrays
- An array of derived classes can implicitly "decay" to a pointer to a base class with potential disastrous results.
- Example: Assume that `Apple` and `Pear` are two kinds of `Fruit`s.
```cpp
void maul(Fruit* p) {
    *p = Pear{};   // put a Pear into *p
    p[1] = Pear{}; // put a Pear into p[1]
}
Apple aa [] = { an_apple, another_apple };   // aa contains Apples (obviously!)
maul(aa);
Apple& a0 = &aa[0];   // a Pear?
Apple& a1 = &aa[1];   // a Pear?
```
- Probably, `aa[0]` will be a Pear (without the use of a cast!). If `sizeof(Apple) != sizeof(Pear)` the access to `aa[1]` will not be aligned to the proper start of an object in the array.
- We have a type violation and possibly (probably) a memory corruption. Never write such code.
- Note that `maul()` violates the a `T*` points to an individual object rule.
- Alternative: Use a proper (templatized) container:

```cpp
void maul2(Fruit* p)
{
    *p = Pear{};   // put a Pear into *p
}
vector<Apple> va = { an_apple, another_apple };   // va contains Apples (obviously!)
maul2(va);       // error: cannot convert a vector<Apple> to a Fruit*
maul2(&va[0]);   // you asked for it

Apple& a0 = &va[0];   // a Pear?
```
- Note that the assignment in `maul2()` violated the no-slicing rule.

## T.82: Linearize a hierarchy when virtual functions are undesirable
- empty session

## T.83: Do not declare a member function template `virtual`
- C++ does not support that. We need a rule because people keep asking about this.
- **If it did, vtbls could not be generated until link time.** And in general, **implementations must deal with dynamic linking.**
```cpp
class Shape {
    // ...
    template<class T>
    virtual bool intersect(T* p);   // error: template cannot be virtual
};
```
- Alternative: Double dispatch, visitors, calculate which function to call

## T.84: Use a non-template core implementation to provide an ABI-stable interface
- Improve stability of code. Avoid code bloat.
- Example: It could be a base class:
```cpp
struct Link_base { // stable
    Link_base* suc;
    Link_base* pre;
};

template <typename T> // templated wrapper to add type safety
struct Link : Link_base {
    T val;
};

struct List_base {
    Link_base* first; // first element (if any)
    int sz;           // number of elements
    void add_front(Link_base* p);
    // ...
};

template <typename T> class List : List_base {
  public:
    void put_front(const T& e) {
        add_front(new Link<T>{e});
    } // implicit cast to Link_base
    T& front() {
        static_cast<Link<T>*>(first).val;
    } // explicit cast back to Link<T>
    // ...
};

List<int> li;
List<string> ls;
```
- Now there is only one copy of the operations linking and unlinking elements of a List.
- The Link and List classes do nothing but type manipulation.
- Instead of using a separate "base" type, another common technique is to specialize for void or void* and have the general template for `T` be just the safely-encapsulated casts to and from the core void implementation.
- Alternative: Use a Pimpl implementation.


