#pragma once

#include "mediator.h"
#include <string>
#include <string_view>
#include <map>
namespace mediator {

class User;
class ChatRoom {// ChatRoom class is the mediator
    std::map<std::string, User*> users_;
public:
    void broadcast(std::string_view originator, std::string_view msg);
    void dm(std::string_view originator, std::string_view tgt, std::string_view msg);
    void logOn(User&);
};
class User
{
    std::string name_;
    std::string logPrefix_;
    ChatRoom& room_;
public:
    User(std::string_view name, ChatRoom& room);
    std::string_view name() { return name_; }
    void receive(std::string_view origin, std::string_view msg);
    void say(std::string_view msg);
    void dm(std::string_view tgt, std::string_view msg);
};

void demo();
} // namespace mediator