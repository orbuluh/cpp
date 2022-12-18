#include "commandUseCase.h"
#include <iostream>

namespace command {
void demo() {
    Chef chef;
    Waiter waiter(chef);
    waiter.clientOrderSteak();
    waiter.clientOrderBeefNoodle();
}
} // namespace command