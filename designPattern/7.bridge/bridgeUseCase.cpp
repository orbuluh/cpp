#include "bridgeUseCase.h"
#include <iostream>

namespace bridge {

class SecreteStrategy::SecreteStrategyImpl {
public:
    bool shouldBuy() { return true; }
};

SecreteStrategy::SecreteStrategy() : pimpl_(new SecreteStrategyImpl()) {}
bool SecreteStrategy::shouldBuy() { return pimpl_->shouldBuy(); }

void demo() {
    SecreteStrategy stgy;
    std::cout << stgy.shouldBuy() << std::endl;

    RasterRenderer rr;
    VectorRenderer vr;
    Circle circle1{rr, 1, 1, 5};
    circle1.draw();
    circle1.resize(2);
    circle1.draw();
    Circle circle2{vr, -1,-1, 3};
    circle2.draw();
    circle2.resize(2);
    circle2.draw();
}
} // namespace bridge