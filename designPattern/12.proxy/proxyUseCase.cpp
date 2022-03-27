#include "proxyUseCase.h"
#include <iostream>

namespace proxy {
struct Order {
    Property<int> qty;
    Property<float> price;
};

void demo() {
    Order order{10, 1.0};
    order.qty = 100;
    auto x = order.qty;
    std::cout << x << '\n';
    
}
} // namespace proxy