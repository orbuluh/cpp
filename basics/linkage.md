# Linkage

> Somewaht formally: **A name that denotes object, reference, function, type, template, namespace, or value, may have linkage.**
> - If a name has linkage, it refers to the **same entity as the same name introduced by a declaration in another scope.**
> - If a variable, function, or another entity with the same name is declared **in several scopes, but does not have sufficient linkage, then several instances of the entity are generated.**

- The concept of linkage is not specific to the C++ programming language.
  - It is a general programming concept that applies to various programming languages.
  - Different programming languages may have their own specific rules and terminology regarding linkage, but the underlying concept remains the same.

- The details and behavior of linkage may vary between programming languages.
  - For example, C and C++ have specific rules for internal and external linkage, while other languages like Java or Python may have different mechanisms for controlling name visibility and accessibility.

- However, the core idea of linkage, which determines **how names are associated and accessed within a program, is applicable to many programming languages**.
  - It is a fundamental concept that helps ensure proper name usage, avoid conflicts, and facilitate code organization and reuse across different scopes, files, or modules.

- What is linkage?
  - Linkage refers to the association between a name and the entity it represents (variable, function, type, etc.) within a program.
  - It determines whether the name can be referenced from other parts of the program and how it interacts with names of the same kind declared in different scopes or files.

- Why do we need linkage?
  - Linkage is important for organizing code and ensuring the correct usage of names. It helps prevent naming conflicts when different parts of the program use the same name for different entities.
  - Additionally, linkage enables code reuse by allowing names to be shared across different scopes or files.

## Types of linkage:

There are different types of linkage that describe the visibility and accessibility of names. Here's a simplified overview:

### No linkage

- Names with no linkage are only accessible within the scope (block of code) where they are declared. They cannot be referenced from other parts of the program.
- Examples include variables declared within a function or names like typedefs and enumerations declared within a block.
- Any of the following names declared at block scope have no linkage:
  - variables that aren't explicitly declared `extern` (regardless of the `static` modifier);
  - local classes and their member functions;
  - other names declared at block scope such as typedefs, enumerations, and enumerators.
  - Names not specified with external, module, (since C++20) or internal linkage also have no linkage, regardless of which scope they are declared in.


### Internal linkage

- Names with internal linkage can be accessed within the same file (translation unit) where they are defined. They are not visible to other files.
  - The name can be referred to **from all scopes in the current translation unit.**
- Internal linkage is useful when you want to define entities that should be private or limited to a specific file.
- Examples include static variables and functions declared at the global scope or variables declared as const within a namespace. Names declared within an unnamed namespace also have internal linkage.

- Any of the following names declared at **namespace scope** have internal linkage:
  - variables, variable templates (since C++14), functions, or function templates declared static;
  - non-volatile non-template (since C++14) non-inline (since C++17) non-exported (since C++20)
  - const-qualified variables (including `constexpr`) (since C++11) that aren't declared `extern` and aren't previously declared to have external linkage;
  - data members of anonymous unions.
  - In addition, **all names declared in unnamed namespace or a namespace within an unnamed namespace, even ones explicitly declared `extern`, have internal linkage**.

### External linkage

- (since C++11)
- The name can be referred to from the scopes **in the other translation units.**
- Variables and functions with external linkage also have **language linkage**, which makes it possible to **link translation units written in different programming languages.**
- Names with external linkage can be accessed from other files (translation units).
- They allow sharing the same name across different parts of the program, enabling communication between different files or modules.
- Examples include global variables and functions that are not declared as static. Names of classes, enumerations, and templates also have external linkage.
- Any of the following names declared at namespace scope have external linkage, unless they are declared in an **unnamed namespace** or their **declarations are attached to a named module and are not exported** (since C++20):
  - variables and functions not listed above (that is, functions not declared static, non-const variables not declared static, and any variables declared `extern`);
  - enumerations;
  - names of classes, their member functions, static data members (const or not), nested classes and enumerations, and functions first introduced with `friend` declarations inside class bodies;
  - names of all templates not listed above (that is, not function templates declared `static`).
- Any of the following names first declared at block scope have external linkage:
    - names of variables declared `extern`;
    - names of functions.
    - module linkage
- The name can be referred to **only from the scopes in the same module unit** or **in the other translation units of the same named module**.
- Names declared at namespace scope have module linkage if their declarations are attached to a named module and are not exported, and don't have internal linkage.

### Module linkage

- Introduced in C++20, module linkage restricts the usage of a name to the same module or other translation units within the same named module.
- It provides a way to control the visibility of names within a module.
- Names attached to a named module and not exported have module linkage. Names with module linkage do not have internal linkage.

# Related

- [Translation unit](translation_unit.md)