#include "maybeMonadUseCase.h"
#include <iostream>

namespace maybemonad {
void printDogNameIfPersonHave(Person* p) {
    auto printName = [](std::string* n) { std::cout << *n << std::endl; };
    maybe(p)
    .With([](auto p) { return p->dog; }) // p is Maybe<Person>
    .With([](auto d) { return d->name; }) // d is Maybe<Dog>
    .Do(printName); //Maybe<Dog>.Do(printName)
}

void demo()
{
    std::string yo = "yo";
    Dog d{&yo};
    Person p{&d};
    printDogNameIfPersonHave(&p);
    Person p2; // noDog
    printDogNameIfPersonHave(&p2); // do nothing
}
} // namespace maybemonad