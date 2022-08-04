## Discussion: Define and initialize member variables in the order of member declaration
- Member variables are always initialized in the order they are declared in the class definition, so write them in that order in the constructor initialization list.
- Writing them in a different order just makes the code confusing because it won't run in the order you see, and that can make it hard to see order-dependent bugs.

```cpp
class Employee {
    string email, first, last;
public:
    Employee(const char* firstName, const char* lastName);
    // ...
};

Employee::Employee(const char* firstName, const char* lastName)
  : first(firstName),
    last(lastName),
    // BAD: first and last not yet constructed
    email(first + "." + last + "@acme.com")
{}
```

- In this example, email will be constructed before first and last because it is declared first.
- That means its constructor will attempt to use first and last too soon -- not just before they are set to the desired values, but before they are constructed at all.
- If the class definition and the constructor body are in separate files, the long-distance influence that the order of member variable declarations has over the constructor's correctness will be even harder to spot.

# Discussion: Use of =, {}, and () as initializers
... empty

# Discussion: Use a factory function if you need "virtual behavior" during initialization
- If your design wants virtual dispatch into a derived class from a base class constructor or destructor for functions like `f` and `g`, you need other techniques, such as a post-constructor -- a separate member function the caller must invoke to complete initialization, which can safely call `f` and `g` because in member functions virtual calls behave normally.
- Some techniques for this are shown in the References.
- Here's a non-exhaustive list of options:
  - Pass the buck: Just document that user code must call the post-initialization function right after constructing an object.
  - Post-initialize lazily: Do it during the first call of a member function. A Boolean flag in the base class tells whether or not post-construction has taken place yet.
  - Use virtual base class semantics: Language rules dictate that the **constructor of the most-derived class decides which base constructor will be invoked; you can use that to your advantage.** (See [Taligent94].)
  - Use a factory function: This way, you can easily force a mandatory invocation of a post-constructor function.

- Here is an example of the last option:
```cpp
class B {
  public:
    B() {
        /* ... */
        f(); // BAD: C.82: Don't call virtual functions in constructors and
             // destructors
        /* ... */
    }

    virtual void f() = 0;
};
```
```cpp
class B {
  protected:
    class Token {};

  public:
    // constructor needs to be public so that make_shared can access it.
    // protected access level is gained by requiring a Token.
    explicit B(Token) { /* ... */
    }                   // create an imperfectly initialized object
    virtual void f() = 0;

    template <class T>
    static shared_ptr<T> create() // interface for creating shared objects
    {
        auto p = make_shared<T>(typename T::Token{});
        p->post_initialize();
        return p;
    }

  protected:
    virtual void post_initialize() // called right after construction
    {                              /* ... */
        f();                       /* ... */
    }                              // GOOD: virtual dispatch is safe
};

class D : public B { // some derived class
  protected:
    class Token {};

  public:
    // constructor needs to be public so that make_shared can access it.
    // protected access level is gained by requiring a Token.
    explicit D(Token) : B{B::Token{}} {}
    void f() override{/* ...  */};

  protected:
    template <class T> friend shared_ptr<T> B::create();
};

shared_ptr<D> p = D::create<D>(); // creating a D object
```
- This design requires the following discipline:
  - Derived classes such as D must not expose a publicly callable constructor. Otherwise, D's users could create D objects that don't invoke `post_initialize`.
  - Allocation is limited to operator new.
  - B can, however, override new (see Items 45 and 46 in SuttAlex05).
  - D must define a constructor with the same parameters that B selected.
  - Defining several overloads of create can assuage this problem, however; and the overloads can even be templated on the argument types.
- If the requirements above are met, the design guarantees that `post_initialize` has been called for any fully constructed B-derived object.
- `post_initialize` doesn't need to be virtual; it can, however, invoke virtual functions freely.

- In summary, no post-construction technique is perfect.
  - The worst techniques dodge the whole issue by simply asking the caller to invoke the post-constructor manually.
  - Even the best require a different syntax for constructing objects (easy to check at compile time) and/or cooperation from derived class authors (impossible to check at compile time).

# Discussion: Usage of `noexcept`
empty

# Discussion: Destructors, deallocation, and swap must never fail
- Never allow an error to be reported from a destructor, a resource deallocation function (e.g., operator `delete`), or a `swap` function using `throw`.
- It is nearly impossible to write useful code if these operations can fail, and even if something does go wrong it nearly never makes any sense to retry.
- Specifically, types whose destructors might throw an exception are flatly forbidden from use with the C++ Standard Library. Most destructors are now implicitly `noexcept` by default.

```cpp
class Nefarious {
public:
    Nefarious() { /* code that could throw */ }    // ok
    ~Nefarious() { /* code that could throw */ }   // BAD, should not throw
    // ...
};
```
- `Nefarious` objects are hard to use safely even as local variables:
```cpp
 void test(string& s)
 {
     Nefarious n;          // trouble brewing
     string copy = s;      // copy the string
 } // destroy copy and then n
 ```
- Here, copying s could throw, and if that throws and if n's destructor then also throws, the program will exit via `std::terminate` because **two exceptions can't be propagated simultaneously.**
- Classes with `Nefarious` members or bases are also hard to use safely, because their destructors must invoke `Nefarious`' destructor, and are similarly poisoned by its bad behavior:

```cpp
 class Innocent_bystander {
     Nefarious member;     // oops, poisons the enclosing class's destructor
     // ...
 };

 void test(string& s)
 {
     Innocent_bystander i;  // more trouble brewing
     string copy2 = s;      // copy the string
 } // destroy copy and then i
 ```
- Here, if constructing copy2 throws, we have the same problem because i's destructor now also can throw, and if so we'll invoke `std::terminate`.
- You can't reliably create global or static `Nefarious` objects either:
```cpp
static Nefarious n;       // oops, any destructor exception can't be caught
```

- You can't reliably create arrays of `Nefarious`:
```cpp
 void test()
 {
     std::array<Nefarious, 10> arr; // this line can std::terminate()
 }
 ```
- **The behavior of arrays is undefined in the presence of destructors that throw because there is no reasonable rollback behavior that could ever be devised.**
  - Just think: What code can the compiler generate for constructing an arr where, if the fourth object's constructor throws, the code has to give up and in its cleanup mode tries to call the destructors of the already-constructed objects ... and one or more of those destructors throws? There is no satisfactory answer.

- You can't use `Nefarious` objects in standard containers:
```cpp
std::vector<Nefarious> vec(10);   // this line can std::terminate()
```
- The standard library forbids all destructors used with it from throwing. You can't store `Nefarious` objects in standard containers or use them with any other part of the standard library.

Note: These are key functions that must not fail because they are necessary for the two key operations in transactional programming:
- to back out work if problems are encountered during processing, and to commit work if no problems occur.
- **If there's no way to safely back out using no-fail operations, then no-fail rollback is impossible to implement.**
- **If there's no way to safely commit state changes using a no-fail operation (notably, but not limited to, `swap`), then no-fail commit is impossible to implement.**

- Consider the following advice and requirements found in the C++ Standard:
  - If a destructor called during stack unwinding exits with an exception, `terminate` is called (15.5.1). So **destructors should generally catch exceptions and not let them propagate out of the destructor**. --[C++03] §15.2(3)
  - No destructor operation defined in the C++ Standard Library (including the destructor of any type that is used to instantiate a standard-library template) will throw an exception. --[C++03] §17.4.4.8(3)

- Deallocation functions, including specifically overloaded operator `delete` and operator `delete[]`, fall into the same category, because they too are used during cleanup in general, and during exception handling in particular, to back out of partial work that needs to be undone.
- Besides destructors and deallocation functions, common error-safety techniques rely also on `swap` operations never failing -- in this case, not because they are used to implement a **guaranteed rollback**, but because they are used to implement a **guaranteed commit**.
- For example, here is an idiomatic implementation of `operator=` for a type `T` that performs copy construction followed by a call to a no-fail `swap`:

```cpp
T& T::operator=(const T& other)
{
    auto temp = other;
    swap(temp);
    return *this;
}
```

- Fortunately, when releasing a resource, the scope for failure is definitely smaller. If using exceptions as the error reporting mechanism, make sure such functions handle all exceptions and other errors that their internal processing might generate.
- (For exceptions, simply wrap everything sensitive that your destructor does in a `try/catch(...)` block.)
- This is particularly important because a destructor might be called in a crisis situation, such as failure to allocate a system resource (e.g., memory, files, locks, ports, windows, or other system objects).
- **When using exceptions as your error handling mechanism, always document this behavior by declaring these functions `noexcept`**. (See Item 75.)

# Define Copy, move, and destroy consistently
- **Note: If you define a copy constructor, you must also define a copy assignment operator.**
- **Note: If you define a move constructor, you must also define a move assignment operator.**

```cpp
class X {
public:
    X(const X&) { /* stuff */ }

    // BAD: failed to also define a copy assignment operator

    X(x&&) noexcept { /* stuff */ }

    // BAD: failed to also define a move assignment operator

    // ...
};

X x1;
X x2 = x1; // ok
x2 = x1;   // pitfall: either fails to compile, or does something suspicious
```
- **If you define a destructor, you should not use the compiler-generated copy or move operation; you probably need to define or suppress copy and/or move.**

```cpp
class X {
    HANDLE hnd;
    // ...
public:
    ~X() { /* custom stuff, such as closing hnd */ }
    // suspicious: no mention of copying or moving -- what happens to hnd?
};

X x1;
X x2 = x1; // pitfall: either fails to compile, or does something suspicious
x2 = x1;   // pitfall: either fails to compile, or does something suspicious
```
- If you define copying, and any base or member has a type that defines a move operation, you should also define a move operation.

```cpp
class X {
    string s; // defines more efficient move operations
    // ... other data members ...
public:
    X(const X&) { /* stuff */ }
    X& operator=(const X&) { /* stuff */ }

    // BAD: failed to also define a move construction and move assignment
    // (why wasn't the custom "stuff" repeated here?)
};

X test()
{
    X local;
    // ...
    return local;  // pitfall: will be inefficient and/or do the wrong thing
}
```
- If you define any of the copy constructor, copy assignment operator, or destructor, you probably should define the others.
- Note: If you need to define any of these five functions, it means you need it to do more than its default behavior -- and the five are asymmetrically interrelated. Here's how:

- **If you write/disable either of the copy constructor or the copy assignment operator, you probably need to do the same for the other:** If one does "special" work, probably so should the other because the two functions should have similar effects. (See Item 53, which expands on this point in isolation.)
- **If you explicitly write the copying functions, you probably need to write the destructor**: If the "special" work in the copy constructor is to allocate or duplicate some resource (e.g., memory, file, socket), you need to deallocate it in the destructor.
- **If you explicitly write the destructor, you probably need to explicitly write or disable copying**: If you have to write a non-trivial destructor, it's often because you need to manually release a resource that the object held. If so, it is likely that those resources require careful duplication, and then you need to pay attention to the way objects are copied and assigned, or disable copying completely.

- In many cases, holding properly encapsulated resources using RAII "owning" objects can eliminate the need to write these operations yourself. (See Item 13.)

- Prefer compiler-generated (including `=default`) special members;
  - only these can be classified as "trivial", and at least one major standard library vendor heavily optimizes for classes having trivial special members. This is likely to become common practice.

- Exceptions:
  - **When any of the special functions are declared only to make them non-public or `virtual`, but without special semantics, it doesn't imply that the others are needed.**
  - In rare cases, classes that have members of strange types (such as reference members) are an exception because they have peculiar copy semantics.
  - In a class holding a reference, you likely need to write the copy constructor and the assignment operator, but the default destructor already does the right thing.
  - (Note that using a reference member is almost always wrong.)

# Discussion: Provide strong resource safety; that is, never leak anything that you think of as a resource
- Prevent leaks. Leaks can lead to performance degradation, mysterious error, system crashes, and security violations.
- Alternative formulation: **Have every resource represented as an object of some class managing its lifetime.**

```cpp
template<class T>
class Vector {
private:
    T* elem;   // sz elements on the free store, owned by the class object
    int sz;
    // ...
};
```
- This class is a resource handle. It manages the lifetime of the Ts.
- To do so, Vector must define or delete the set of special operations (constructors, a destructor, etc.).

# Discussion: Never return or throw while holding a resource not owned by a handle
- That would be a leak.

```cpp
void f(int i)
{
    FILE* f = fopen("a file", "r");
    ifstream is { "another file" };
    // ...
    if (i == 0) return;
    // ...
    fclose(f);
}
```
- If i == 0 the file handle for a file is leaked. On the other hand, the `ifstream` for another file will correctly close its file (upon destruction). 
- If you must use an explicit pointer, rather than a resource handle with specific semantics, use a `unique_ptr` or a `shared_ptr` with a custom deleter:

```cpp
void f(int i)
{
    unique_ptr<FILE, int(*)(FILE*)> f(fopen("a file", "r"), fclose);
    // ...
    if (i == 0) return;
    // ...
}
```
- Better:
```cpp
void f(int i)
{
    ifstream input {"a file"};
    // ...
    if (i == 0) return;
    // ...
}
```

# Discussion: A "raw" pointer or reference is never a resource handle
- To be able to distinguish owners from views.
- Note: This is independent of how you "spell" pointer: `T*`, `T&`, `Ptr<T>` and `Range<T>` are not owners.

# Discussion: Never let a pointer outlive the object it points to
- Reason: To avoid extremely hard-to-find errors.
- Dereferencing such a pointer is undefined behavior and could lead to violations of the type system.

```cpp
string* bad() // really bad
{
    vector<string> v = {"This", "will", "cause", "trouble", "!"};
    // leaking a pointer into a destroyed member of a destroyed object (v)
    return &v[0];
}

void use() {
    string* p = bad();
    vector<int> xx = {7, 8, 9};
    // undefined behavior: x might not be the string "This"
    string x = *p;
    // undefined behavior: we don't know what (if anything) is allocated a
    // location p
    *p = "Evil!";
}
```
- The strings of `v` are destroyed upon exit from `bad()` and so is `v` itself.
- The returned pointer points to unallocated memory on the free store. This memory (pointed into by `p`) might have been reallocated by the time `*p` is executed. There might be no string to read and a write through `p` could easily corrupt objects of unrelated types.


# Discussion: Use templates to express containers (and other resource handles)
- To provide statically type-safe manipulation of elements.

```cpp
template<typename T> class Vector {
    // ...
    T* elem;   // point to sz elements of type T
    int sz;
};
```

# Discussion: Return containers by value (relying on move or copy elision for efficiency)
- To simplify code and eliminate a need for explicit memory management. To bring an object into a surrounding scope, thereby extending its lifetime.
- See also: F.20, the general item about "out" output values

```cpp
vector<int> get_large_vector()
{
    return ...;
}
auto v = get_large_vector(); //  return by value is ok, most modern compilers will do copy elision
```
- Exception: See the Exceptions in [F.20](F.call.md#f20-for-out-output-values-prefer-return-values-to-output-parameters).


# Discussion: If a class is a resource handle, it needs a constructor, a destructor, and copy and/or move operations
- To provide **complete control of the lifetime of the resource**.
- To provide **a coherent set of operations on the resource.**

- Note: If all members are resource handles, rely on the default special operations where possible.

```cpp
template<typename T> struct Named {
    string name;
    T value;
};
```
- Now `Named` has a default constructor, a destructor, and efficient copy and move operations, provided `T` has.

# Discussion: If a class is a container, give it an initializer-list constructor
- It is common to need an initial set of elements.

```cpp
template<typename T> class Vector {
public:
    Vector(std::initializer_list<T>);
    // ...
};
Vector<string> vs { "Nygaard", "Ritchie" };
```