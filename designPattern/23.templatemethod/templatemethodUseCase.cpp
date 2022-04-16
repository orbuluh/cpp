#include "templatemethodUseCase.h"
#include <iostream>

namespace templatemethod {

void SimplifiedHumanBehavior::behave(Event e) {
    prepare(e);
    execute(e);
    conclude(e);
}

void demo() {
    Extravert p1;
    p1.behave(Event::Gathering);
    std::cout << "\n";
    p1.behave(Event::Quarantine);
    std::cout << "\n";

    Intravert p2;
    p2.behave(Event::Gathering);
    std::cout << "\n";
    p2.behave(Event::Quarantine);
    std::cout << "\n";
}
} // namespace templatemethod