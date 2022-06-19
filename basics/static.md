# Storage class specifiers - `static`

- `static` or `thread` storage duration and internal linkage (or external linkage for static class members not in an anonymous namespace).

- The static specifier is **only allowed in
  - the declarations of **objects** (except in function parameter lists),
  - declarations of **functions** (except at block scope), and
  - declarations of **anonymous unions**.

- When used in a declaration of a **class member**, it declares a **static member**.
- When used in a declaration of an **object**, it specifies **static storage duration** (except if accompanied by `thread_local`).
- When used in a declaration at **namespace scope,** it specifies **internal linkage**.

## static storage duration.
- The storage for the object is allocated **when the program begins** and **deallocated when the program ends.**
- **Only one instance of the object exists**.
- **All objects declared at namespace scope (including global namespace) have this storage duration**, **plus those declared with `static` or `extern`**.
- See Non-local variables and Static local variables for details on initialization of objects with this storage duration.

## Static local variables
- Variables declared at block scope with the specifier `static` or `thread_local` (since C++11) have static or thread (since C++11) storage duration but are **initialized the first time control passes through their declaration (unless their initialization is zero- or constant-initialization, which can be performed before the block is first entered)**.
- On all further calls, the declaration is skipped.
- If the initialization throws an exception, the variable is not considered to be initialized, and **initialization will be attempted again the next time control passes through the declaration.**
- If the initialization recursively enters the block in which the variable is being initialized, the behavior is undefined.
- If multiple threads attempt to initialize the same static local variable concurrently, **the initialization occurs exactly once** (similar behavior can be obtained for arbitrary functions with `std::call_once`).

- Note: **usual implementations of this feature use variants of the double-checked locking pattern, which reduces runtime overhead for already-initialized local statics to a single non-atomic boolean comparison**.
- (since C++11) The destructor for a block-scope static variable is called at program exit, but only if the initialization took place successfully.

- Function-local static objects in all definitions of the same `inline` function (which may be implicitly `inline`) all refer to the **same object defined in one translation unit**, as long as the function has external linkage.


## initialization of non-local variables with static storage duration
- All non-local variables with static storage duration are initialized as part of program startup, before the execution of the main function begins (unless deferred, see below).
- All non-local variables with thread-local storage duration are initialized as part of thread launch, sequenced-before the execution of the thread function begins.
- For both of these classes of variables, **initialization occurs in two distinct stages:**
- **Static initialization**
  - There are two forms of static initialization:
  - 1) If possible, constant initialization is applied.
  - 2) Otherwise, non-local `static` and `thread-local` variables are **zero-initialized**.
  - In practice:
    - Constant initialization is usually applied at compile time.
    - Pre-calculated object representations are stored as part of the program image.
    - If the compiler doesn't do that, it must still guarantee that the initialization happens before any dynamic initialization.
    - Variables to be zero-initialized are placed in the `.bss` segment of the program image, which occupies no space on disk and is zeroed out by the OS when loading the program.
- **Dynamic initialization**
  - After all static initialization is completed, dynamic initialization of non-local variables occurs in the following situations:
    - (1) **Unordered dynamic initialization**
      - which applies only to (`static`/`thread-local)` class template static data members and variable templates (since C++14) that aren't explicitly specialized.
      - Initialization of such static variables is **indeterminately sequenced** with respect to all other dynamic initialization except if the program starts a thread before a variable is initialized, in which case its initialization is unsequenced (since C++17).
      - Initialization of such thread-local variables is unsequenced with respect to all other dynamic initialization.
    - (2) **Partially-ordered dynamic initialization**,
      - which applies to all inline variables that are not an implicitly or explicitly instantiated specialization.
      - If a partially-ordered V is defined before ordered or partially-ordered W in every translation unit, the initialization of V is sequenced before the initialization of W (or happens-before, if the program starts a thread). (since C++17)
    - (3) **Ordered dynamic initialization**,
      - which applies to all other non-local variables: within a single translation unit, initialization of these variables is always sequenced in **exact order their definitions appear in the source code**.
      - **Initialization of `static` variables in different translation units is indeterminately sequenced**.
      - **Initialization of thread-local variables in different translation units is unsequenced.**
      - If the initialization of a non-local variable with static or thread storage duration exits via an exception, `std::terminate` is called.
