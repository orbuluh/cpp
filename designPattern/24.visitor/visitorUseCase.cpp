#include "visitorUseCase.h"
#include <iostream>

namespace visitor {
void demo() {
    Residential residential;
    Bank bank;
    CoffeeShop coffeeshop;

    GenericVisitor<InsuranceSeller,
                   Residential, Bank, CoffeeShop> visitor{"Orb", 1234};

    visitor.visit(residential);
    visitor.visit(bank);
    visitor.visit(coffeeshop);
}
} // namespace visitor