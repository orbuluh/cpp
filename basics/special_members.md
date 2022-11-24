# Special members

- Notes taking from [foonathan::​blog()](https://www.foonathan.net/2019/02/special-member-functions/)
- Quick reference from the [foonathan::​blog()](https://www.foonathan.net/special-member-chart/)

## Prerequisite concepts

A **“user-declared” special member function** is a special member function that is **in any way mentioned in the class**:

- It can have a definition, it can be `default`ed, it can be deleted.
- This means that writing `foo(const foo&) = default` prohibits a move constructor.

A **compiler declared “defaulted” special member** behaves the same as `= default`, e.g. a defaulted copy constructor copy constructs all members.

A **compiler declared “deleted” special member** behaves the same as `= delete`, e.g. if **overload resolution decides to use that overload it will fail with an error** that you are invoking a deleted function.

If a compiler does **not** declare a special member, it **does not participate in overload resolution**

- This is different from a deleted member, which does participate.
- For example, if you have a copy constructor, the compiler will not declare move constructor.
- As such, writing `T obj(std::move(other))` will result in a call to a copy constructor.
- If on the other hand the move constructor were deleted, writing that would select the move constructor and then error because it is deleted.

> TO-CHECK: The behavior of the boxes marked red is deprecated, as the defaulted behavior in that case is dangerous.

---

## Majority of Cases: Rule of Zero

```cpp
class normal
{
public:
    // rule of zero
};
```

- The absolute majority of classes do not need a destructor. Then you also don’t need a copy/move constructor or copy/move assignment operator: The compiler generated defaults do the right thing.
- If you don’t have any constructors, the class will have a compiler generated default constructor. If you have a constructor, it will not. In that case add a default constructor if there is a sensible default value.
- It is often not a good idea to introduce an artificial “null” state, just use `std::optional<T>` instead.

---

## Container Classes: Rule of Five (Six)

```cpp
class container
{
public:
    container() noexcept;
    ~container() noexcept;

    container(const container& other);
    container(container&& other) noexcept;

    container& operator=(const container& other);
    container& operator=(container&& other) noexcept;
};
```

- If you need to write a destructor — because you have to free dynamic memory, for example — **the compiler generated copy constructor and assignment operator will do the wrong thing. Then you have to provide your own.**

- This is known as the rule of five. Whenever you have a custom destructor, also write a copy constructor and assignment operator that have matching semantics. -
- For performance reasons also write a move constructor and move assignment operator.
  - The move functions can steal the resources of the original objects and leave it in an empty state. Strive to make them `noexcept` and fast.
- As you now have a constructor, there will not be an implicit default constructor.
- In most cases it makes sense to implement a default constructor that puts the class in the empty state, like the post-move one.
- This makes it the rule of six.

---

## Resource Handle Classes: Move-only

```cpp
class resource_handle
{
public:
    resource_handle() noexcept;
    ~resource_handle() noexcept;

    resource_handle(resource_handle&& other) noexcept;
    resource_handle& operator=(resource_handle&& other) noexcept;

    // resource_handle(const resource_handle&) = delete;
    // resource_handle& operator=(const resource_handle&) = delete;
};
```

- Sometimes you need to write a destructor but cannot implement a copy.
- An example would be class that wraps a file handle or a similar OS resource.
- Make those classes **move-only**. In other words: write a destructor and move constructor and assignment operators.
- If you look at Howard’s chart, you’ll see that in that case the **copy constructor and assignment operators are deleted**. This is correct, as the class should be move-only. If you want to be explicit, you can also manually `= delete` them.
- Again, **it makes sense to add a default constructor that puts it in the post-move state.**

---

## Immoveable Classes

```cpp

class immoveable
{
public:
    immoveable(const immoveable&) = delete;
    immoveable& operator=(const immoveable&) = delete;

    // immoveable(immoveable&&) = delete;
    // immoveable& operator=(immoveable&&) = delete;
};
```

- Sometimes you want that a class **cannot be copied or moved**. Once an object is created it will always stay at that address. **This is convenient if you want to safely create pointers to that object**.

- In that case you want to delete your copy constructor. The compiler will then not declare a move constructor, meaning all kinds of copying or moving will try to invoke the copy constructor, which is deleted. If you want to be explicit, you can also manually `= delete` it.

- You should also delete the assignment operator. **While it does not physically move the object, assignment is closely related to the constructors**, see below.

---

## Avoid: Rule of Three

```cpp
class avoid
{
public:
    ~avoid();

    avoid(const avoid& other);
    avoid& operator=(const avoid& other);
};
```

- **If you implement only copy operations, moving a class will still invoke copy.**
- **Lots of generic code assumes that a move operations is cheaper than a copy, so try to respect that.**
- If you have C++11 support, implement move for a performance improvement.

---

## Don’t: Copy-Only Types

```cpp
class dont
{
public:
    ~dont();

    dont(const dont& other);
    dont& operator=(const dont& other);

    dont(dont&&) = delete;
    dont& operator=(dont&&) = delete;
};
```

- If you have copy operations and manually delete the move operations, they will still participate in overload resolution.
- This is surprising, so don’t do that. For example:

```cpp
dont a(other);            // okay
dont b(std::move(other)); // error: calling deleted function
```

---

## Don’t: Deleted Default Constructor

```cpp
class dont
{
public:
    dont() = delete;
};
```

- There is no reason to `= delete` a default constructor, if you don’t want one, write another one.
- The only exception would be a type that cannot be constructed in any way, but such a type isn’t really useful without language support for “bottom” or “never” types. So just don’t do it.

---

## Don’t: Partial Implementation

```cpp
class dont
{
public:
    dont(const dont&);
    dont& operator=(const dont&) = delete;
};
```

- The following also applies to move construction and move assignment.
- Copy construction and copy assignment are a pair. You either want both or none.
- **Conceptually, copy assignment is just a faster “destroy + copy construct” cycle.**
- So if you have copy construct, you should also have copy assignment, as it can be written using a destructor call and construction anyway.

- Generic code often requires that type can be copied. If it is not carefully crafted, it might not make a distinction between copy construction and copy assignment.

- While there can be philosophical arguments for a type that can only be copy constructed and not assigned or vice-versa, do the pragmatic thing and avoid them.


```cpp
Consider: Swap
class consider
{
public:
    friend void swap(consider& lhs, consider& rhs) noexcept;
};
```

- Some algorithms, especially pre-move ones, use swap() to move objects around.
- If your type does not provide a `swap()` that can be found via ADL, it will use std::swap().
- `std::swap()` does three moves:

```cpp
template <typename T>
void swap(T& lhs, T& rhs)
{
    T tmp(std::move(lhs));
    lhs = std::move(rhs);
    rhs = std::move(tmp);
}
```

- If you can implement a faster `swap()`, do it.
- Of course, this only applies to classes that have a custom destructor, where you’ve implemented your own copy or move.
- Your own `swap()` should always be `noexcept`.
