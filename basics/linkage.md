# Linkage
- **A name that denotes object, reference, function, type, template, namespace, or value, may have linkage.**
- If a name has linkage, it refers to the **same entity as the same name introduced by a declaration in another scope.**
- If a variable, function, or another entity with the same name is declared **in several scopes, but does not have sufficient linkage, then several instances of the entity are generated.**

- The following linkages are recognized:

## no linkage
- The name **can be referred to only from the scope it is in.**
- Any of the following names declared at block scope have no linkage:
  - variables that aren't explicitly declared `extern` (regardless of the `static` modifier);
  - local classes and their member functions;
  - other names declared at block scope such as typedefs, enumerations, and enumerators.
  - Names not specified with external, module, (since C++20) or internal linkage also have no linkage, regardless of which scope they are declared in.

## internal linkage
- The name can be referred to **from all scopes in the current translation unit.**
- Any of the following names declared at **namespace scope** have internal linkage:
  - variables, variable templates (since C++14), functions, or function templates declared static;
  - non-volatile non-template (since C++14) non-inline (since C++17) non-exported (since C++20)
  - const-qualified variables (including `constexpr`) (since C++11) that aren't declared `extern` and aren't previously declared to have external linkage;
  - data members of anonymous unions.
  - In addition, **all names declared in unnamed namespace or a namespace within an unnamed namespace, even ones explicitly declared `extern`, have internal linkage**.

## external linkage
- (since C++11)
- The name can be referred to from the scopes **in the other translation units.**
- Variables and functions with external linkage also have **language linkage**, which makes it possible to **link translation units written in different programming languages.**

- Any of the following names declared at namespace scope have external linkage, unless they are declared in an **unnamed namespace** or their **declarations are attached to a named module and are not exported** (since C++20):
  - variables and functions not listed above (that is, functions not declared static, non-const variables not declared static, and any variables declared `extern`);
  - enumerations;
  - names of classes, their member functions, static data members (const or not), nested classes and enumerations, and functions first introduced with `friend` declarations inside class bodies;
  - names of all templates not listed above (that is, not function templates declared `static`).
- Any of the following names first declared at block scope have external linkage:
    - names of variables declared `extern`;
    - names of functions.
    - module linkage
- The name can be referred to only from the scopes in the same module unit or in the other translation units of the same named module.
- Names declared at namespace scope have module linkage if their declarations are attached to a named module and are not exported, and don't have internal linkage.

# Related
- [Translation unit](translation_unit.md)