#include "visitorUseCase.h"
#include <iostream>

namespace visitor {
void demo() {
    CoffeeShop cs;
    InsuranceSeller sells;
    sells.accept(cs);
}
} // namespace visitor