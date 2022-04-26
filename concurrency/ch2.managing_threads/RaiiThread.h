#pragma once

#include <memory>
#include <thread>

namespace managing::threads::examples {

class RaiiThread {
    std::unique_ptr<std::thread> t_;
public:
    template <typename Fnc>
    void start(Fnc fnc) {
        t_ = std::make_unique<std::thread>(fnc);
    }
    RaiiThread() = default;
    ~RaiiThread() {
        cleanup();
    }

    RaiiThread(const RaiiThread&) = delete;
    RaiiThread& operator=(const RaiiThread&) = delete;

    RaiiThread(RaiiThread&& rhs) {
        cleanup();
        t_ = std::move(rhs.t_);
    }

    RaiiThread& operator=(RaiiThread&& rhs) {
        cleanup();
        t_ = std::move(rhs.t_);
        return *this;
    }

private:
    void cleanup() {
        if (t_ && t_->joinable()) {
            t_->join();
            t_.reset();
        }
    }
};

} // managing::threads::examples