# C.hier: Class hierarchies (OOP)

## C.121: If a base class is used as an interface, make it a pure abstract class
- A class is more stable if it does not contain data. Interfaces should normally be composed **entirely of public pure virtual functions and a default/empty virtual destructor.**

## C.133: Avoid protected data
- `protected` data is a source of complexity and errors. protected data complicates the statement of invariants.
- `protected` data inherently violates the guidance against putting data in base classes, which usually leads to having to deal with virtual inheritance as well.
- It is up to every derived class to manipulate the protected data correctly. This has been popular, but also a major source of maintenance problems.
  - In a large class hierarchy, the consistent use of protected data is hard to maintain because there can be a lot of code, spread over a lot of classes.
  - The set of classes that can touch that data is open: anyone can derive a new class and start manipulating the protected data.
  - Often, it is not possible to examine the complete set of classes, so any change to the representation of the class becomes infeasible.
  - There is no enforced invariant for the protected data; it is much like a set of global variables. The protected data has de facto become global to a large body of code.
  - Protected data often looks tempting to enable arbitrary improvements through derivation. Often, what you get is unprincipled changes and errors. Prefer private data with a well-specified and enforced invariant. Alternative, and often better, [keep data out of any class used as an interface](#c121-if-a-base-class-is-used-as-an-interface-make-it-a-pure-abstract-class).

