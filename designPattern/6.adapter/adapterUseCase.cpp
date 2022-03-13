#include "adapterUseCase.h"
#include <iostream>

namespace adapter {
void demo() {
    SwitchController<PokemonUnite> game1;
    game1.pressA();
    game1.pressB();
    game1.pressX();
    game1.pressY();
    SwitchController<PokemonArceus> game2;
    game2.pressA();
    game2.pressB();
    game2.pressX();
    game2.pressY();
}
} // namespace adapter