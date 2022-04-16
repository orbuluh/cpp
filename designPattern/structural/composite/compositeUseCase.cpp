#include "compositeUseCase.h"
#include <iostream>

namespace composite {
void demo() {
    auto birthdayPresent = std::make_unique<Box>("birthdayPresent");
    birthdayPresent->addItem(prototype::GoodsFactory::create(prototype::Item::NikePg5EpSonyPS5Edition));
    birthdayPresent->addItem(prototype::GoodsFactory::create(prototype::Item::NikeBasketBallSocks));
    birthdayPresent->addItem(prototype::GoodsFactory::create(prototype::Item::NikeBasketBallSocks));
    auto forHoo = std::make_unique<Box>("forHoo");
    forHoo->addItem(prototype::GoodsFactory::create(prototype::Item::NikeWaffleDebut));
    auto fedexPackage = std::make_unique<Box>("WholePackage");
    fedexPackage->addBox(std::move(birthdayPresent));
    fedexPackage->addBox(std::move(forHoo));
    std::cout << fedexPackage->value() << std::endl;
}
} // namespace composite