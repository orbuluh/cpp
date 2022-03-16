#include "facadeUseCase.h"
#include <iostream>

namespace facade {
struct ComplexLib {
    void feature1() { std::cout << "feature1" << std::endl; };
    void feature2() { std::cout << "feature2" << std::endl; };
    void feature3() { std::cout << "feature3" << std::endl; };
    //...
    void featureX() { std::cout << "feature4" << std::endl; };
};

struct Facade {
    void onlyUseFeature1OutOfComplexLib() {
        complexLib.feature1();
    }
    ComplexLib complexLib;
};

void demo() {
    Facade f;
    f.onlyUseFeature1OutOfComplexLib();
}

} // namespace facade