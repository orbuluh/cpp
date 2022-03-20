#pragma once

#include "command.h"
#include <map>
#include <iostream>
#include <memory>
namespace command {

struct Chef {
    // Business logic is here
    void cook(std::string_view dishName) {
        std::cout << "Chef cooking " << dishName << '\n';
    }
};

struct ICookCommand {
    ICookCommand(Chef& chef) : chef_(chef) {}
    virtual ~ICookCommand() = default;
    virtual void order() = 0;
    Chef& chef_;
};

struct CookSteak : ICookCommand {
    CookSteak(Chef& chef) : ICookCommand(chef) {}
    ~CookSteak() override = default;
    void order() override {
        chef_.cook("steak");
    }
};

struct CookBeefNoodle : ICookCommand {
    CookBeefNoodle(Chef& chef) : ICookCommand(chef) {}
    ~CookBeefNoodle() override = default;
    void order() override {
        chef_.cook("beef noodle");
    }
};

struct Waiter {
    Waiter(Chef& chef) : chef_(chef) {
        menu_["steak"] = std::move(std::make_unique<CookSteak>(chef_));
        menu_["beef noodle"] = std::move(std::make_unique<CookBeefNoodle>(chef_));
    }
    void clientOrderSteak() {
        menu_["steak"]->order();
    }
    void clientOrderBeefNoodle() {
        menu_["beef noodle"]->order();
    }

    Chef& chef_;
    std::map<std::string, std::unique_ptr<ICookCommand>> menu_;
};

void demo();
} // namespace command