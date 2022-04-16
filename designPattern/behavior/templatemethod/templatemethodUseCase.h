#pragma once
#include <iostream>

namespace templatemethod {

enum class Event {
    Gathering,
    Quarantine,
};

class SimplifiedHumanBehavior {
public:
    virtual ~SimplifiedHumanBehavior() = default;
    void behave(Event e);
protected:
    virtual void prepare(Event e) = 0;
    virtual void execute(Event e) = 0;
    virtual void conclude(Event e) = 0;
};

class Extravert : public SimplifiedHumanBehavior {
public:
    ~Extravert() override = default;
    void prepare(Event e) override {
        switch (e) {
        case Event::Gathering:
            std::cout << "It's going so much fun.\n"; break;
        case Event::Quarantine:
            std::cout << "OMG I don't want to do this...\n"; break;
        default:
            break;
        }
    }
    void execute(Event e) override {
        switch (e) {
        case Event::Gathering:
            std::cout << "Hang out freely in the party.\n"; break;
        case Event::Quarantine:
            std::cout << "Let me out dang.\n"; break;
        default:
            break;
        }
    }
    void conclude(Event e) override {
        switch (e) {
        case Event::Gathering:
            std::cout << "Feel even more energetic after the party\n"; break;
        case Event::Quarantine:
            std::cout << "I am not going to travel and do this again.\n"; break;
        default:
            break;
        }
    }
};

class Intravert : public SimplifiedHumanBehavior {
public:
    ~Intravert() override = default;
    void prepare(Event e) override {
        switch (e) {
        case Event::Gathering:
            std::cout << "I'm so going to stay in the corner and just drink.\n"; break;
        case Event::Quarantine:
            std::cout << "Alright I guess acceptable...\n"; break;
        default:
            break;
        }
    }
    void execute(Event e) override {
        switch (e) {
        case Event::Gathering:
            std::cout << "Silently drinking at the corner\n"; break;
        case Event::Quarantine:
            std::cout << "Do my own stuff like usual, sometimes stare at the window...\n"; break;
        default:
            break;
        }
    }
    void conclude(Event e) override {
        switch (e) {
        case Event::Gathering:
            std::cout << "Omg my greate night for inner pieace is ruined, I need another day off to regenarate\n"; break;
        case Event::Quarantine:
            std::cout << "Not as bad as I googled... finally I can go home and do the same thing\n"; break;
        default:
            break;
        }
    }
};

void demo();

} // namespace templatemethod