#pragma once

#include "maybeMonad.h"
#include <string>

struct Dog {
    std::string* name{};
};
struct Person {
    Dog* dog{};
};
void printDogNameIfPersonHave(Person* p);