#include "chainofrespUseCase.h"
#include <iostream>

namespace chainofresp {

struct DummyPasswordCheck : public UserCheck {
    DummyPasswordCheck() = default;
    ~DummyPasswordCheck() override = default;
    bool check(User& user) override {
        if (user.getPassword() != "backdoorpass!")
            return false;
        std::cout << "Password checked!" << std::endl;
        return UserCheck::check(user);
    }
};

struct DummyIpCheck : public UserCheck {
    DummyIpCheck() = default;
    ~DummyIpCheck() override = default;
    bool check(User& user) override {
        if (user.getSourceIp() != "0.0.0.0")
            return false;
        std::cout << "IP checked!" << std::endl;
        return UserCheck::check(user);
    }
};


void demo() {

    DummyPasswordCheck check;
    DummyIpCheck subCheck;
    check.add(dynamic_cast<UserCheck*>(&subCheck));

    User experimentalUser("0.0.0.0", "backdoorpass!");
    User actualUser("255.254.253.252", "yo");
    bool res = check.check(experimentalUser);
    std::cout << res << std::endl;
    res = check.check(actualUser);
    std::cout << res << std::endl;
}
} // namespace chainofresp