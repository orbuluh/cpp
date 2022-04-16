# Background
In C++ specifically, we can use any of the following:
- Using nullptr to encode absence.
- Using a smart pointer (e.g., shared_ptr) which, again, can be tested for presence or absence.
- std::optional<T> is a library solution; it can either store a value of type T or std::nullopt if the value is missing.

In “conventional” C++ we would implement it like so:

```c++
void printDogNameIfPersonHave(Person* p) {
    if (p != nullptr
        && p->dog != nullptr
        && p->dog->name != nullptr) // ugh!
        cout << *p->dog->name << endl;
}
```

Using maybeMonad, we eventually do ...

```c++
void printDogNameIfPersonHave(Person* p) {
    auto printName = [](std::string* n) { std::cout << *n << std::endl; };
    maybe(p)
    .With([](auto p) { return p->dog; }) // p is Maybe<Person>
    .With([](auto d) { return d->name; }) // d is Maybe<Dog>
    .Do(printName); //Maybe<Dog>.Do(printName)
}
```

# Some more explanation from other source
- [1](https://stackoverflow.com/a/194207/4924135)
    * The term monad is a bit vacuous if you are not a mathematician. An alternative term is computation builder which is a bit more descriptive of what they are actually useful for.
    * They are a pattern for chaining operations.
    * The gist of the pattern is we have a type which has a method which takes a function as argument. The operation supplied must return an instance of the same type.
    * ...