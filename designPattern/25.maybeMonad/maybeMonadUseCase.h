#pragma once

#include "maybeMonad.h"
#include <string>
namespace maybemonad {
struct Dog {
    std::string* name{};
};
struct Person {
    Dog* dog{};
};
void printDogNameIfPersonHave(Person* p);
void demo();
} // namespace maybemonad