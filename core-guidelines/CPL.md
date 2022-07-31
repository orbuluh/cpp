# CPL: C-style programming
- C and C++ are closely related languages. They both originate in "Classic C" from 1978 and have evolved in ISO committees since then.
- Many attempts have been made to keep them compatible, but **neither is a subset of the other.**

## CPL.1: Prefer C++ to C
- C++ provides better type checking and more notational support.
- It provides better support for high-level programming and often generates faster code.

```cpp
char ch = 7;
void* pv = &ch;
int* pi = pv;   // not C++
*pi = 999;      // overwrite sizeof(int) bytes near &ch
```
- The rules for implicit casting to and from void* in C are subtle and unenforced. In particular, this example violates a **rule against converting to a type with stricter alignment.**


## CPL.2: If you must use C, use the common subset of C and C++, and compile the C code as C++
- That subset can be compiled with both C and C++ compilers, and when compiled as C++ is better type checked than "pure C."

```cpp
int* p1 = malloc(10 * sizeof(int));                      // not C++
int* p2 = static_cast<int*>(malloc(10 * sizeof(int)));   // not C, C-style C++
int* p3 = new int[10];                                   // not C
int* p4 = (int*) malloc(10 * sizeof(int));               // both C and C++
```
- The C++ compiler will enforce that the code is valid C++ unless you use C extension options.

## CPL.3: If you must use C for interfaces, use C++ in the calling code using such interfaces
- C++ is more expressive than C and offers better support for many types of programming.
- For example, to use a 3rd party C library or C systems interface
  - define the low-level interface in the common subset of C and C++ for better type checking.
  - Whenever possible encapsulate the low-level interface in an interface that follows the C++ guidelines (for better abstraction, memory safety, and resource safety) and use that C++ interface in C++ code.

- Example: You can call C from C++:
```cpp
// in C:
double sqrt(double);

// in C++:
extern "C" double sqrt(double);

sqrt(2);
```

- Example: You can call C++ from C:
```cpp
// in C:
X call_f(struct Y*, int);

// in C++:
extern "C" X call_f(Y* p, int i) {
    return p->f(i); // possibly a virtual function call
}
```