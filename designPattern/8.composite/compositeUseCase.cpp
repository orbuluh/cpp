#include "compositeUseCase.h"
#include <iostream>

namespace composite {
void demo() {
    auto birthdayPresent = std::make_unique<Box>("birthdayPresent");
    birthdayPresent->addItem("NikePg5EpSonyPS5Edition", 1357.0);
    birthdayPresent->addItem("NikeBasketBallSocks", 79);
    birthdayPresent->addItem("NikeBasketBallSocks", 79);
    auto forHoo = std::make_unique<Box>("forHoo");
    forHoo->addItem("NikeWaffleDebut", 599);
    auto fedexPackage = std::make_unique<Box>("WholePackage");
    fedexPackage->addBox(std::move(birthdayPresent));
    fedexPackage->addBox(std::move(forHoo));
    std::cout << fedexPackage->value() << std::endl;
}
} // namespace composite