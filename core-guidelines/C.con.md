# C.con: Containers and other resource handles
- See also [Resource management](R.md)

- [C.con: Containers and other resource handles](#ccon-containers-and-other-resource-handles)
  - [C.100: Follow the STL when defining a container](#c100-follow-the-stl-when-defining-a-container)
  - [C.101: Give a container value semantics](#c101-give-a-container-value-semantics)
  - [C.102: Give a container move operations](#c102-give-a-container-move-operations)
  - [C.103: Give a container an initializer list constructor](#c103-give-a-container-an-initializer-list-constructor)
  - [C.104: Give a container a default constructor that sets it to empty](#c104-give-a-container-a-default-constructor-that-sets-it-to-empty)
  - [C.109: If a resource handle has pointer semantics, provide `*` and `->`](#c109-if-a-resource-handle-has-pointer-semantics-provide--and--)

## C.100: Follow the STL when defining a container
- There are of course other fundamentally sound design styles and sometimes reasons to depart from the style of the standard library, but in the absence of a solid reason to differ, it is simpler and easier for both implementers and users to follow the standard.

## C.101: Give a container value semantics
- Regular objects are simpler to think and reason about than irregular ones. Familiarity.
- If meaningful, make a container Regular (the concept). In particular, ensure that an object compares equal to its copy.
```cpp
void f(const Sorted_vector<string>& v)
{
    Sorted_vector<string> v2 {v};
    if (v != v2)
        cout << "Behavior against reason and logic.\n";
    // ...
}
```

## C.102: Give a container move operations
- Containers tend to get large; without a move constructor and a copy constructor an object can be expensive to move around, thus tempting people to pass pointers to it around and getting into resource management problems.

## C.103: Give a container an initializer list constructor
- People expect to be able to initialize a container with a set of values. Familiarity.

## C.104: Give a container a default constructor that sets it to empty
- To make it `Regular`.

## C.109: If a resource handle has pointer semantics, provide `*` and `->`
- That's what is expected from pointers. Familiarity.
