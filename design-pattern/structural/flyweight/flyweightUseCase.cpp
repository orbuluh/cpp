#include "flyweightUseCase.h"
#include <iostream>

namespace flyweight {
void demo() {
    MapBuilder builder(5, 5);
    builder.build(0, 0, TextureType::Rock);
    builder.build(0, 1, TextureType::Rock);
    builder.build(0, 2, TextureType::Grass);
    builder.build(0, 3, TextureType::Grass);
    builder.build(0, 2, TextureType::Ground);
    builder.build(1, 2, TextureType::Ground);
    builder.build(2, 2, TextureType::Ground);
    builder.build(3, 2, TextureType::Ground);
    builder.draw();
}
} // namespace flyweight