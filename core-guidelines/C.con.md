# C.con: Containers and other resource handles
- See also [Resource management](R.md)

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