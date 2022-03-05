#include "designPattern/maybeMonadUseCase.h"
#include <ranges>
#include <iostream>

void maybeMonandUseCase()
{
    std::string yo = "yo";
    Dog d{&yo};
    Person p{&d};
    printDogNameIfPersonHave(&p);
    Person p2; // noDog
    printDogNameIfPersonHave(&p2); // do nothing
}

int main()
{
    maybeMonandUseCase();
}
