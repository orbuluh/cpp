# T: Templates and generic programming

- Generic programming is programming using types and algorithms parameterized by types, values, and algorithms.
- In C++, generic programming is supported by the template language mechanisms.
- Arguments to generic functions are **characterized by sets of requirements** on the argument types and values involved.
- In C++, these requirements are expressed by **compile-time predicates called concepts.**
- Templates can also be used for meta-programming; that is, programs that compose code at compile time.
- A central notion in generic programming is "concepts"; that is, requirements on template arguments presented as compile-time predicates.

# Subsections
- [T.gp: Generic programming](T.gp.md)
- [T.concepts: Concept rules](T.concept.md)
- [T.interface Templates interfaces](T.interface.md)
- [T.def: Template definitions](T.def.md)
- [T.hier: Template and hierarchy rules](T.hire.md)

# T.var: Variadic template rules
- (mostly to be added)

## T.100: Use variadic templates when you need a function that takes a variable number of arguments of a variety of types
- Variadic templates is the most general mechanism for that, and is both efficient and type-safe. Don't use C varargs.

## T.101: ??? How to pass arguments to a variadic template ???
- beware of move-only and reference arguments

## T.102: How to process arguments to a variadic template

## T.103: Don't use variadic templates for homogeneous argument lists
- There are more precise ways of specifying a homogeneous sequence, such as an `initializer_list`.