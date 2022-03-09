#include "observerUseCase.h"
#include <iostream>

namespace observer {

struct Baby : Observable<Baby> {
    void cry() {
        std::cout << "Baby cry" << std::endl;
        notify(*this, "cry");
    }

    void calm() {
        std::cout << "Baby calm" << std::endl;
    }

    void angry() {
        std::cout << "Baby angry" << std::endl;
    }
};

struct Mom : Observer<Baby> {
    void eventTriggered(Baby& baby, std::string_view description) {
        std::cout << "Mom: sing because baby " << description << std::endl;
        baby.calm();
    }
};

struct Dad : Observer<Baby> {
    void eventTriggered(Baby& baby, std::string_view description) {
        std::cout << "Dad: panic because baby " << description << std::endl;
        baby.angry();
    }
};

void demo() {
    Baby baby;
    Mom mom;
    Dad dad;
    baby.subscribe(&mom);
    baby.subscribe(&dad);
    baby.cry();
}
} // namespace observer