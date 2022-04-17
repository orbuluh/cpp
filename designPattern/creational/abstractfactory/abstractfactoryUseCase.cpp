#include "abstractfactoryUseCase.h"
#include <iostream>

namespace abstractfactory {
void demo() {
    std::cout << "AB testing experimental animation against production ready animation\n";
    ProductionAnimation paFactory;
    NbaPlayer a(paFactory);
    a.play();
    ExperimentalAnimation expFactory;
    NbaPlayer b(expFactory);
    b.play();
    // you can also create a mock factory and return all mock stuff!
}
} // namespace abstractfactory