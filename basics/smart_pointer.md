# smart_pointer

> Taking notes from [fluent C++](https://www.fluentcpp.com/2018/12/25/free-ebook-smart-pointers/)

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

## `std::shared_ptr`

- A single memory resource can be held by several `​std::shared_ptr​s` at the same time​.
- The `​shared_ptr​s` internally maintain a count of how many of them there are holding the same resource, and when the last one is destroyed, it deletes the memory resource.
- Therefore `​std::shared_ptr` allows copies, but with a reference-counting mechanism to make sure that every resource is deleted once and only once.

`std::shared_ptr`​ should not be used by default​, for several reasons:

- Having several simultaneous holders of a resource makes for a **​more complex** system than with one unique holder, like with `​std::unique_ptr`​.
- Having several simultaneous holders of a resource makes **​thread-safety​ harder**,
- It makes the **​code counter-intuitive** when an object is not shared in terms of the domain and still appears as "shared" in the code for a technical reason,
- It can incur a **​performance cost, both in time and memory**, because of the bookkeeping related to the reference-counting.

One good case for using ​std::shared_ptr though is when objects are ​shared in the domain​.

- Using shared pointers then reflects it in an expressive way. Typically, the nodes of a graphs are well represented as shared pointers, because several nodes can hold a reference to one other node.


## `std::weak_ptr`

- `std::weak_ptr​s` can hold a reference to a shared object along with other `std::shared_ptr`​s, but **they don't increment the reference count.**
- This means that if no more `std::shared_ptr` are holding an object, this object will be deleted even if some weak pointers still point to it.
- For this reason, a weak pointer needs to check if the object it points to is still alive. To do this, it **has to be copied into to a `​std::shared_ptr`**​:

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
- Another use case is that `​weak_ptr` can be used to **maintain a cache**​: The data may or may not have been cleared from the cache, and the
`weak_ptr​` references this data.

## `scoped_ptr`

- It simply **disables the copy and even the move construction**.
- So it is the **sole owner of a resource, and its ownership cannot be transferred.**
- Therefore, **a `​scoped_ptr` can only live inside a scope. Or as a data member of an object.**
- And of course, as a smart pointer, it keeps the advantage of deleting its underlying pointer in its destructor.


