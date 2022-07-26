# Pro: Profiles

- Ideally, we would follow all of the guidelines. That would give the cleanest, most regular, least error-prone, and often the fastest code.
  - Unfortunately, that is usually impossible because we have to fit our code into large code bases and use existing libraries.
  - Often, such code has been written over decades and does not follow these guidelines. We must aim for gradual adoption.

- Whatever strategy for **gradual adoption** we adopt, we need to be able to apply sets of related guidelines to address some set of problems first and leave the rest until later.
  - A similar idea of "related guidelines" becomes important when some, but not all, guidelines are considered relevant to a code base or if a set of specialized guidelines is to be applied for a specialized application area.
- We call such a set of related guidelines a "**profile**".
  - We aim for such a set of guidelines to be coherent so that they together help us reach a specific goal, such as "absence of range errors" or "static type safety."
  - Each profile is designed to eliminate a class of errors.
  - Enforcement of "random" rules in isolation is more likely to be disruptive to a code base than delivering a definite improvement.

- A "profile" is a set of deterministic and portably enforceable subset of rules (i.e., restrictions) that are designed to achieve a specific guarantee.
- **"Deterministic"** means they require only local analysis and could be implemented in a compiler (though they don't need to be).
- **"Portably enforceable"** means they are like language rules, so programmers can count on different enforcement tools giving the same answer for the same code.

- Code written to be warning-free using such a language profile is considered to **conform** to the profile.
  - Conforming code is considered to be safe by construction with regard to the safety properties targeted by that profile.
  - Conforming code will not be the root cause of errors for that property, although such errors might be introduced into a program by other code, libraries or the external environment.
  - A profile might also introduce additional library types to ease conformance and encourage correct code.


- In the future, we expect to define many more profiles and add more checks to existing profiles. Candidates include:
  - narrowing arithmetic promotions/conversions (likely part of a separate safe-arithmetic profile)
  - arithmetic cast from negative floating point to unsigned integral type (ditto)
  - selected undefined behavior: Start with Gabriel Dos Reis's UB list developed for the WG21 study group
  - selected unspecified behavior: Addressing portability concerns.
  - const violations: Mostly done by compilers already, but we can catch inappropriate casting and underuse of const.

- Enabling a profile is implementation defined; typically, it is set in the analysis tool used.

- To suppress enforcement of a profile check, place a suppress annotation on a language contract. For example:
```cpp
[[suppress(bounds)]] char* raw_find(char* p, int n, char x)    // find x in p[0]..p[n - 1]
{
    // ...
}
```
- Now raw_find() can scramble memory to its heart's content. Obviously, suppression should be very rare.

# Pro.safety: Type-safety profile
- This profile makes it easier to construct code that uses types correctly and avoids inadvertent type punning.
- It does so by focusing on removing the primary sources of type violations, including unsafe uses of casts and unions.

- For the purposes of this section, type-safety is defined to be the property that **a variable is not used in a way that doesn't obey the rules for the type of its definition.**
- Memory accessed as a type `T` should not be valid memory that actually contains an object of an unrelated type `U`.
- Note that the safety is intended to be complete when combined also with Bounds safety and Lifetime safety.
- An implementation of this profile shall recognize the following patterns in source code as non-conforming and issue a diagnostic.

## Type.1: Avoid casts:
- **Don't use `reinterpret_cast`;**
  - A strict version of Avoid casts and prefer named casts.
- **Don't use `static_cast` for arithmetic types;**
  - A strict version of Avoid casts and prefer named casts.
- Don't cast between pointer types where the source type and the target type are the same;
  - A strict version of Avoid casts.
- **Don't cast between pointer types when the conversion could be implicit;**
  - A strict version of Avoid casts.

## Type.2: Don't use `static_cast` to downcast: Use `dynamic_cast` instead.

## Type.3: Don't use `const_cast` to cast away const (i.e., at all): Don't cast away const.

## Type.4: Don't use C-style `(T)` expression or functional `T(expression)` casts: Prefer construction or named casts or `T{expression}`.

## Type.5: Don't use a variable before it has been initialized: always initialize.

## Type.6: Always initialize a member variable: always initialize, possibly using default constructors or default member initializers.

## Type.7: Avoid naked union: Use variant instead.

## Type.8: Avoid `varargs`: Don't use `va_arg` arguments.

- Impact:
- With the type-safety profile you can trust that every operation is applied to a valid object.
- An exception can be thrown to indicate errors that cannot be detected statically (at compile time).
- Note that this type-safety can be complete only if we also have Bounds safety and Lifetime safety. Without those guarantees, a region of memory could be accessed independent of which object, objects, or parts of objects are stored in it.

# Pro.bounds: Bounds safety profile
- This profile makes it easier to construct code that operates within the bounds of allocated blocks of memory.
- It does so by focusing on removing the primary sources of bounds violations: pointer arithmetic and array indexing.
- One of the core features of this profile is to restrict pointers to only refer to single objects, not arrays.

- We define bounds-safety to be the property that a program does not use an object to access memory outside of the range that was allocated for it.
- Bounds safety is intended to be **complete only when combined with Type safety and Lifetime safety**, which cover other unsafe operations that allow bounds violations.

## Bounds.1: Don't use pointer arithmetic. Use span instead: Pass pointers to single objects (only) and Keep pointer arithmetic simple.
## Bounds.2: Only index into arrays using constant expressions: Pass pointers to single objects (only) and Keep pointer arithmetic simple.
## Bounds.3: No array-to-pointer decay: Pass pointers to single objects (only) and Keep pointer arithmetic simple.
## Bounds.4: Don't use standard-library functions and types that are not bounds-checked: Use the standard library in a type-safe manner.

- Impact
  - Bounds safety implies that access to an object - notably arrays - does not access beyond the object's memory allocation.
  - This eliminates a large class of insidious and hard-to-find errors, including the (in)famous "buffer overflow" errors.
  - This closes security loopholes as well as a prominent source of memory corruption (when writing out of bounds).
  - **Even if an out-of-bounds access is "just a read", it can lead to invariant violations (when the accessed isn't of the assumed type) and "mysterious values."**

# Pro.lifetime: Lifetime safety profile
- Accessing through a pointer that doesn't point to anything is a major source of errors, and very hard to avoid in many traditional C or C++ styles of programming.
- For example, a pointer might be uninitialized, the `nullptr`, point beyond the range of an array, or to a deleted object.

## Lifetime.1: Don't dereference a possibly invalid pointer: detect or avoid.

- Impact:
  - Once completely enforced through a combination of style rules, static analysis, and library support, this profile eliminates one of the major sources of nasty errors in C++
  - eliminates a major source of potential security violations
  - improves performance by eliminating redundant "paranoia" checks
  - increases confidence in correctness of code
  - avoids undefined behavior by enforcing a key C++ language rule