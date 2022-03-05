#include "maybeMonadUseCase.h"
#include <iostream>

void printDogNameIfPersonHave(Person* p) {
    auto printName = [](std::string* n) { std::cout << *n << std::endl; };
    maybe(p)
    .With([](auto p) { return p->dog; }) // p is Maybe<Person>
    .With([](auto d) { return d->name; }) // d is Maybe<Dog>
    .Do(printName); //Maybe<Dog>.Do(printName)
}