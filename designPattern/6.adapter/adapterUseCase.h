#pragma once
#include <iostream>
namespace adapter {

template <typename GameProfile>
struct SwitchController { // e.g. the Adapter
    // XBox controller:
    //    X
    //  Y   A
    //    B
    void pressA() { GameProfile::pressA(); }
    void pressB() { GameProfile::pressB(); }
    void pressX() { GameProfile::pressX(); }
    void pressY() { GameProfile::pressY(); }
    SwitchController() {
        std::cout << "Playing: " << GameProfile::name() << std::endl;
    }
};

struct PokemonUnite {
    static std::string name() { return "PokemonUnite"; }
    static void pressA() { std::cout << "Attack" << std::endl; }
    static void pressB() { std::cout << "Cancel Move" << std::endl; }
    static void pressX() { std::cout << "Score" << std::endl; }
    static void pressY() { std::cout << "Battle Item" << std::endl; }
};

struct  PokemonArceus{
    static std::string name() { return "PokemonArceus"; }
    static void pressA() { std::cout << "Investigate or talk" << std::endl; }
    static void pressB() { std::cout << "Crounch or rise" << std::endl; }
    static void pressX() { std::cout << "Ready an item or Pokemon" << std::endl; }
    static void pressY() { std::cout << "Dodge" << std::endl; }
};

void demo();
} // namespace adapter