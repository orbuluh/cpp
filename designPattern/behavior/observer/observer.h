#pragma once
#include <string_view>
#include <vector>

template<typename T>
struct Observer {
    virtual void eventTriggered(T& source, std::string_view description) = 0;
};

template<typename T>
struct Observable {
    using ObservingT = Observer<T>;
    void notify(T& source, std::string_view name) {
        for (auto obs : observers_) {
            obs->eventTriggered(source, name);
        }
    }
    void subscribe(ObservingT* observer) { observers_.push_back(observer); }
    void unsubscribe(ObservingT* observer) { observers_.erase(observer); }
private:
    std::vector<ObservingT*> observers_;
};