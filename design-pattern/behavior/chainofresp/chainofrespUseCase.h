#pragma once
#include <iostream>
namespace chainofresp {
class User {
public:
    User(std::string ip, std::string pw) : sourceIp_(ip), password_(pw) {}
    std::string getSourceIp() const { return sourceIp_; }
    std::string getPassword() const { return password_; }
private:
    std::string sourceIp_;
    std::string password_;
};

struct UserCheck {
    UserCheck() = default;
    virtual ~UserCheck() = default;
    void add(UserCheck* next) { next_ = next; }
    virtual bool check(User& user) {
        if (next_) return next_->check(user);
        return true;
    }
    UserCheck* next_ = nullptr;
};

void demo();
} // namespace chainofresp