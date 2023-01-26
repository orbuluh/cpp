# [Copy elision](https://en.cppreference.com/w/cpp/language/copy_elision)

:man_teacher: Omits copy and move (since C++11) constructors, resulting in **zero-copy pass-by-value semantics**.

- Note: Copy elision is the only allowed form of optimization (until C++14) out of the two allowed forms of optimization, alongside allocation elision and extension, (since C++14) that can change the observable side-effects.

- Because some compilers do not perform copy elision in every situation where it is allowed (e.g., in debug mode), **programs that rely on the side-effects of copy/move constructors and destructors are not portable.**

- When copy elision occurs, the implementation treats the source and target of the omitted copy/move (since C++11) operation as **simply two different ways of referring to the same object**, and the destruction of that object occurs at the later of the times **when the two objects would have been destroyed without the optimization**
  - (except that, if the parameter of the selected constructor is an rvalue reference to object type, the destruction occurs when the target would have been destroyed) (since C++11).

- **Multiple copy elisions may be chained to eliminate multiple copies.**


## Mandatory elision of copy/move operations

- The objects are **constructed directly into the storage where they would otherwise be copied/moved to.**
- The copy/move constructors **need not be present or accessible**
- The rule above does **not** specify an optimization: C++17 core language specification of **prvalues and temporaries is fundamentally different** from that of the earlier C++ revisions: there is **no longer a temporary to copy/move from**.
- Another way to describe C++17 mechanics is "**unmaterialized value passing**": **prvalues are returned and used without ever materializing a temporary.**

:one: In a return statement, when the operand is a prvalue of the same class type (ignoring cv-qualification) as the function return type:

:two: In the initialization of an object, when the initializer expression is a prvalue of the same class type (ignoring cv-qualification) as the variable type.

- This can only apply when the object being initialized is known not to be a potentially-overlapping subobject.

(Check [code](copy_elision.h))

## Non-mandatory elision of copy/move (since C++11) operations

- compilers are **permitted, but not required** to omit the copy and move (since C++11) construction of class objects even if the copy/move (since C++11) constructor and the destructor have observable side-effects.
- The objects are constructed directly into the storage where they would otherwise be copied/moved to.
- **This is an optimization: even when it takes place and the copy/move (since C++11) constructor is not called, it still must be present and accessible (as if no optimization happened at all), otherwise the program is ill-formed**.


:one: In a return statement, when the operand is the name of a non-volatile object with automatic storage duration, which isn't a function parameter or a catch clause parameter, and which is of the same class type (ignoring cv-qualification) as the function return type. This variant of copy elision is known as **NRVO, "named return value optimization"**.

:two: In the initialization of an object, when the source object is a **nameless temporary and is of the same class type** (ignoring cv-qualification) as the target object.

- When the nameless temporary is the operand of a return statement, **this variant of copy elision is known as RVO, "return value optimization".** (until C++17) **Return value optimization is mandatory and no longer considered as copy elision**; see above. (since C++17)

:three: In a throw-expression, when the operand is the name of a non-volatile object with automatic storage duration, which isn't a function parameter or a catch clause parameter, and whose scope does not extend past the innermost try-block (if there is a try-block).

:four: In a catch clause, when the argument is of the same type (ignoring cv-qualification) as the exception object thrown, the copy of the exception object is omitted and the body of the catch clause accesses the exception object directly, as if caught by reference (there cannot be a move from the exception object because it is always an lvalue).

- This is disabled if such copy elision would change the observable behavior of the program for any reason other than skipping the copy constructor and the destructor of the catch clause argument (for example, if the catch clause argument is modified, and the exception object is rethrown with throw). (since C++11)

:five: In coroutines, copy/move of the parameter into coroutine state may be elided where this does not change the behavior of the program other than by omitting the calls to the parameter's constructor and destructor.

- This can take place **if the parameter is never referenced after a suspension point or when the entire coroutine state was never heap-allocated in the first place.** (since C++20)

:six: In constant expression and constant initialization, return value optimization (RVO) is guaranteed, however, named return value optimization (NRVO) is forbidden.