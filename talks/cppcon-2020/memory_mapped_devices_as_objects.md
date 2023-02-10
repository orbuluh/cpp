# Memory-Mapped Devices as Objects - Dan Saks

## Device drivers and device registers

- A **device driver** is a software subsystem that controls an **external** device attached to computer
- "External" means outside the CPU, even if it's on the same chip as the CPU.
- CPUs typically communicate with external devices via device registers
- A device register is circuitry that provides an interface to a device.

## Device register functions

A single device may use different registers for different functions.

- A **control** register:
  - Configures the device
  - Initiates an operation
- A **status** register:
  - provides information about the device's state
- A **transmit** register:
  - sends a data value to the device
- A **receive** register:
  - receives a data value from the device

## Sample device registers

- An example single-board computer which a UART (serial port) employs 6 device registers

|Offset|Register|Description|
|--|--|--|
|0x00 (0) | ULCON | line control |
|0x04 (4) | UCON  | control |
|0x08 (8) | USTAT | status |
|0x0C (12)| UTXBUF| transmit buffer |
|0x10 (16)| URXBUF| receive buffer |
|0x14 (20)| UBRDIV| baud rate divisor (control) |

## Why call device "register"?

- A device register is a register because it has similarities to...
  - A CPU register or
  - ordinary memory
- In particular, a device register is
  - a collection of bits
  - accessible as a conventional storage unit, such as a byte, or a word

## Memory-mapped registers

- Most modern computer architectures use memory-mapped addressing
- That is, a memory-mapped [device] register ...
  - connects to the CPU's (address and data) bus structure, and
  - responds to bus signals **almost** as if it were ordinary memory
- This enables CPU to communicate with the device without special instructions
- In short, memory-mapped addressing disguises the device registers to be addressable like "ordinary" memory.

## "Typical" address space

![](../pics/memory_mapped_device_typical_address_space.JPG)

- Full box (including dash) is the entire address space
- Usually the amount of memory on the system doesn’t occupy the whole address space
- Typically a small amount of memory in the low memory space stores interrupt vectors
- Memory-mapped registers would be somewhere high in the address space, and usually access relative to some base address

## Easy to use incorrectly (than ordinary memory)

With ordinary memory, you can set clearer individual bytes to store values in there, and it doesn't upset anything else. There is not so much difference other than the bit position between various bits in memory.

But with devices, since each bit in the register, or say, in the control register, has a very specific meaning and has some interaction with the hardware, it becomes very easy to make programming error like ...

- misread the state of a device.
- put a device into an invalid state
- inadvertently trigger an operation

The problem is, when you misuse a device register, the resulting bugs can be

- spurious and
- hard to debug/reproduce

## The technique to cutdown the error rely on common practice

The C tradition is to declare device registers using fairly primitive types, typically:

- integers or
- characters

This is less work up front but it leave too many opportunities for programming errors.

In C++, you should wrap device registers inside classes, specifically

> :man_teacher: Declare device registers as non-public data members

> :man_teacher: Declare public functions with restrictive interface that prevent invalid operations on those registers

This is more work up front, but the resulting device drivers will be simpler and more robust. You pay now or pay later (and when it's later, you usually pay a lot more).

- **You define each device type at most once. The upfront cost of thoughtful class design is not a repeated cost**
- You could easily access each device many times.
  - The inconvenience of a poor interface could be a frequently repeated cost.
  - The upfront cost of a good interface could amortize itself many times over.

> :man_teacher: Make interfaces easy to use correctly and hard to use incorrectly (Scott Meyers)

> :man_teacher: Or in other words ... program in a style that turns potential run-time errors into compile time errors. (Dan Saks)

The following talks is the incremental step to build up a class to model the device with such philosophy.

## Choosing the register type(s)

A good starting point is to declare each device register using an appropriate integer type. For example...

- A 2-byte register might be `uint16_t` from `<cstdint>
- For register occupies a 4-byte word, use `uint32_t` works well.

Using a meaningful type alias is often better style

```cpp
using device_register = uint32_t; // alias declaration in modern c++
```

## Placing ordinary objects in memory

- One huge issue for memory-mapped device is that although you want it to be just like normal object, it's not.
- With ordinary object in memory, the compiler takes the declaration and definition that you write and figures out where those object is going to be.
- Compiler doesn't do all this by itself, sometimes, it gets help from the linker. - Other times, it gets help from the run-time system.
- But it's all done automatically, by these entities, on your behave.

```cpp
int foo() {
  int i;             // automatic duration
  static char *p;    // static duration
}
```

- For `i`, which has automatic storage duration.
  - They will be on the stack. Where exactly will they be? Compiler will figure it out.
  - The compiler sets aside so many bytes at some offset **from the frame pointer.**
  - But where is the frame pointer for compiler to set the offset? It only knows till runtime. The program sets the frame pointer value when it enters the function at runtime.
  - So it's a compile time offset computation plus the placement of the value   of frame pointer at runtime that determines where the object is going to live.
- For `p`, which has static storage duration
  - The compiler sets aside so many bytes at some offset **within a particular data segment.**
  - The linker glues together the data segments, and finally picks the address for the object to reside.

## Placing memory-mapped objects

- For a memory-mapped register, the compiler doesn't get to choose where the object resides because the hardware already determine it.
- C++ has no standard way to define an object to reside at a specified address.
- You have several alternatives...

## Method 1: At-placement

- Some compilers let you declare an object at a specified address, they provide a non-standard syntax such as

```cpp
device_register UTXBUF0 _at(0x03FFD00C);
device_register UTXBUF0 @ 0x03FFD00C;
```
- These notations called at-placement are non-standard.
- In this example, UTXBUF0 is at offset 0xD00C relative to 00x03FF0000, the base address of the device registers.
- Note for above syntax, it's a compile time and link time machinery to specify that the hardware is going to reside there. It doesn't involve runtime computation.
  - On the contrary, placement new is a runtime operation, it executes something at runtime to identify where the device is.
- Using at-placement, you can manipulate the device register as an object, as in

```cpp
UTXBUF0 = c; // OK: send c's value to the serial port
```

- This assignment writes the value of character c to UART 0's transmit buffer.
- The assignment initiates serializing the value and sending it to the serial port.

## Method 2: Linker-placement

- Alternatively, you can declare a memory-mapped object using a standard extern declaration, such as

```cpp
extern device_register UTXBUF0;
```

- This **doesn't define** UTXBUF0, it just **declares** it.
- Again, C++ has no standard notation to define an object at a specific address, you have to do it with the linker.
- For example, the GNU linker accepts a command-line argument like:

```bash
--defsym UTXBUF0=0x3FFD00C
```

- It places the symbol at the desired address.
- Again, the syntax of the extern declaration is valid Standard C++. However, the resulting program is not. You didn't write the definition in Standard C++.
- According to standard, this could actually violate ODR. You declare something you never define in C++. You use some other tool to patch in the address. Regardless, it's still a viable way to do.
- According to measurement, this technique shows the worst performance in several platform. Mainly because compiler doesn't know the addressing at all as it all defers to the linker. So compiler uses worst case scenario to work on this. (E.g. assume this address is far far away and requires certain instructions to do sth etc.)
- This technique, called linker-placement, just pawns off the non-standard stuff to the linker

## Method 3: Pointer-placement

- You can define a pointer to the device register, then you can access the register by that pointer, as in:

```cpp
*UTXBUF0 = c; // OK: send c's value to the serial port
```

- You can use pointer-placement to cast an integer-valued address into a pointer value.
- Using an object-like macro is common in C:

```cpp
#define UTXBUF0 ((device_register*)0x03FFD00C)
```

- Of course, in C++, we favor using:
  - const objects over object-like macros and
  - "new style" casts over "old style" casts

```cpp
device_register* const UTXBUF0
  = reinterpret_cast<device_register *>(0x03FFD00C);
```

- In modern C++, we prefer using constexpr object definition:

```cpp
constexpr device_register* UTXBUF0
  = reinterpret_cast<device_register *>(0x03FFD00C); // compile error
```

- But this doesn't compile ... why? Standard C++ doesn't permit a `reinterpret_cast` in a constant expression
  - Even one with an integer-literal operand
- A `static_cast` can appear in a constant-expression:

```cpp
constexpr device_register* UTXBUF0
  = static_cast<device_register *>(0x03FFD00C); // compile error
```

- But with this particular usage, it still doesn't compile. Because a `static_cast` can't convert an integer to a pointer.

- So the first one is the only way to go currently. Although you want such placement being evaluated in compile time, we can only check it in run time with `reinterpret_cast`.
- Note that `reinterpret_cast` has **implementation-defined behavior**, though. (Behavior could vary from platform to platform.)
  - On many platforms, converting an integer to a (built-in) pointer doesn't transform the bits. E.g. whatever the bit pattern was for the integer, if a pointer is the same size as the integer, it just picks up those bits and put it into the pointer. No runtime cost to it, it's all done in compile time.
  - However, on some platforms, it might. There is no guarantee. There could be truncation or sign extension of bits even if pointer and integer have same size.
- So need to be alerted that although this works in most case, it does have implementation-defined behavior. The result might just differ for different platform. So it's potentially non-portable.

## Reference-placement

- Alternatively, you can define UTXBUF0 as a reference.
  - You can eliminate the `const` when doing so, and in fact you can't even do so. (You can do `T* const`, but you can't do `T& const`)
- However, you can't cast an integer-literal to a reference type.

```cpp
device_register& UTXBUF0 = reinterpret_cast<device_register&>(0x03FFD00C);
                                                         //^ compile error
```

- Instead, you must cast to a pointer type and dereference the result to obtain an object to which the reference can bind.

```cpp
device_register& UTXBUF0 = *reinterpret_cast<device_register*>(0x03FFD00C);
                         //^ need to dereference!
```

- This technique, called reference-placement, lets you treat UTXBUF0 as the device register itself:

```cpp
UTXBUF0 = C; // OK, send c's value to the serial port
```

- This is very close to the convenience of at-placement. But unlike at-placement, it's standard C++. (Though still have implementation defined behavior)

## Passing registers separately to functions is too error-prone

- Many UART operations involve more than one UART register.

```cpp
void UART_put (
  device_register& stat,
  device_register& txbuf,
  char c
);

UART_put(UTXBUF0, USTAT0, c); // wrong order?
UART_put(USTAT0, UTXBUF1, c); // mismatching UART #s
```

## The solution is clustering registers into class

```cpp
class UART {
public:
  bool get(char& c);
  bool put(char c);
private:
  device_register ULCON;
  device_register UCON;
  device_register USTAT;
  device_register UTXBUF;
  device_register URXBUF;
  device_register UBRDIV;
};

void UART_put(UART& uart, char c);
```

- if you use `struct`, it would still be error-prone for misuse
- Declare device registers as non-public class data members
- Declare public functions with restrictive interfaces that prevent invalid operations on those registers


## Unwelcome optimization

- Device registers aren't ordinary memory
- Device register accesses (reads and writes) may have side effect. For example:
  - Writing to a control register may initiate an operation
  - Reading from a receiver buffer may set or clear bits in a a status register
- Compiler optimizations might eliminate register accesses
  - Eliminating accesses eliminates those side effect
  - Eliminating those side effects might cause device drivers to fail

## The volatile qualifier

- Declaring an object `volatile` inhibits optimizations
- In particular, the compiler can't eliminate accesses to `volatile` objects... even when nit seems safe to do so.


## The right dose of volatility

- This declares `com0` as a "const pointer to a volatile UART" (e.g. pointer always points to the same spot, and the thing it's pointing to shall have all its access taken literally. There will be no access optimization.)

```cpp
UART volatile * const com0
  = reinterpret_cast<UART*>(0x03FFD000);
```

- This declares `com1` as a "reference to a volatile UART"

```cpp
UART volatile & com1
  = *reinterpret_cast<UART*>(0x03FFE000);
```

- Note: `volatile` here isn't part of the UART type - so you have to declare it every time. Failure to declare can lead to subtle bugs.

```cpp
UART &com1  // BAD! missing volatile
  = *reinterpret_cast<UAR*>(09x3FFE000);
```

- Again, the compiler might optimize-away some accesses to `com1`'s registers and eliminate needed side effects.
- If every `UART` should be volatile, then volatile should be part of the `UART` type.
- One way could be:

```cpp
class UART {
  bool get(char& c);
  bool put(char c);
private:
  device_register volatile ULCON;
  device_register volatile UCON;
  device_register volatile USTAT;
  device_register volatile UTXBUF;
  device_register volatile URXBUF;
  device_register volatile UBRDIV;
};
```

- But when a device has many registers, this is tedious


- Can we make class volatile?

```cpp
volatile class UART { // error: missing declarator
  //...
};
```

- Compiler w2ants to apply `volatile` to a declarator

```cpp
volatile class UART { // OK, but type UART isn't volatile.
} u;                  // instead, only object u is volatile
```

- You can try defining a class for a non-volatile

```cpp
class nv_uart {
public:
  bool get(char& c);
  bool put(char c);
private:
  device_register ULCON;
  device_register UCON;
  device_register USTAT;
};
//...
using UART = nv_uart volatile;
```

- Unfortunately, this leads to problems calling `nv_uart` member functions applied to UART objects.

```cpp
char c = 'a';
UART &com0 // using UART = nv_uart volatile;
  = *reinterpret_cast<UART*>(0x03FFE000);
com0.put(c); // error: conversion loses qualifier(s)
com0.get(c); // error: conversion loses qualifier(s)
```

- C++ won't let you apply a non-volatile member function to a volatile object ... you have to declare many, if not all, of the nv_uart member functions as volatile:

```cpp
class nv_uart {
public:
  bool get(char& c) volatile;
  bool put(char c) volatile;
private:
  device_register ULCON;
  device_register UCON;
  device_register USTAT;
};
```

- This is tedious ... so what's the recommendation?

## Push volatile to device_register instead

```cpp
using device_register = uint32_t volatile;
```

- Typically, all device registers (not just those in UARTs) are volatile
- In that case, just declare device_register as a volatile type
- The class definition reverts to its earlier simple form:

```cpp
class UART {
public:
  bool get(char& c);
  bool put(char c);
private:
  device_register ULCON;
  device_register UCON;
  device_register USTAT;
  //...
};
```

- so now, you no longer needs define member function as volatile.
- UART isn't volatile anymore, but every non-static UART data member is.
- And now, you have


```cpp

UART* const com0 = reinterpret_cast<UART*>(0x3FFD000);

// or

UART& com1 = *reinterpret_cast<UART*>(0x3FFD000);
```

## "Standard-layout types"

- Next issue, how do you make sure the UART class has the exact layout?
- C++ provides storage layout guarantees, but only for standard layout types.
- Think of standard-layout type as a C type, with a little more.
  - A scalar type: arithmetic, enumeration or pinter type
  - An array with elements of standard-layout type
  - A standard-layout class, possibly declared as a structure or union
- A standard-layout class can also have some real C++ features...
- A standard-layout class have:
  - static and non-static data members: if the non-static data members are all standard-layout types
  - base classes: if they're all standard-layout types
  - static and non-virtual member function
  - nested constants and types
- A standard-layout class **can't** have
  - virtual functions or
  - virtual base classes
- The virtual add extra data that could disturb the layout. E.g. the virtual function pointers go to the beginning of class, it's going to push all the data register further down, and the offset would be incorrect

```cpp
class timer {
public:
  void enable();
  virtual value_type get(); // not standard layout
};
```

- All non-static data members of a standard-layout class must have the same access control
  - There is no guarantee about the ordering of data sections with different access control
  - despite how they declare, it will just disturb the layout.

```cpp
class widget {
public:
  device_register status;
protected:                  // not standard layout
  device_register control;
  device_register data;
};
```


================== tmp @ 41:04==================