#include "mediatorUseCase.h"
#include <iostream>

namespace mediator {

void ChatRoom::broadcast(std::string_view originator, std::string_view msg) {
    for (auto p : users_) {
        if (p.first != originator)
            p.second->receive(originator, msg);
    }
}
void ChatRoom::dm(std::string_view originator, std::string_view tgt, std::string_view msg) {
    auto it = users_.find(std::string{tgt});
    if (it != users_.end()) {
        it->second->receive(originator, msg);
    }
}
void ChatRoom::logOn(User& user) {
    users_.insert({std::string{user.name()}, &user});
}

User::User(std::string_view name, ChatRoom& room)
    : name_(name), room_(room), logPrefix_(std::string{"["} + name_ + "]") {}

void User::receive(std::string_view origin, std::string_view msg) {
    std::cout << logPrefix_ << msg << " from " << origin << std::endl;
}

void User::say(std::string_view msg) {
    room_.broadcast(name_, msg);
}

void User::dm(std::string_view tgt, std::string_view msg) {
    room_.dm(name_, tgt, msg);
}

void demo() {
    ChatRoom chat;
    User a("A", chat);
    User b("B", chat);
    User c("C", chat);
    User d("D", chat);
    chat.logOn(a);
    chat.logOn(b);
    chat.logOn(c);
    chat.logOn(d);
    a.say("yoooo");
    b.dm("A", "heyy");
}
} // namespace mediator