#pragma once
#include <atomic>
#include <iostream>
#include <queue>
#include <thread>

namespace memory_order {

namespace {
std::string work = "notSet";
std::atomic<bool> ready(false);
} // namespace

void consumer(std::memory_order flag) {
    while (!ready.load(flag)) {
    }
    std::cout << work << std::endl;
}

void producer(std::memory_order flag, std::string_view val) {
    work = val;
    ready = true;
}

void demo() {
    using mo = std::memory_order;
    for (const auto& [flagP, flagC, val] : {
             std::make_tuple(mo::seq_cst, mo::seq_cst, "seq_cst"),
             std::make_tuple(mo::acquire, mo::release, "acquire_release"),
             // std::make_tuple(mo::relaxed, mo::relaxed, "relaxed"),
         }) {
        std::thread prod(producer, flagP, val);
        std::thread con(consumer, flagC);
        prod.join();
        con.join();
    }
}

} // namespace memory_order

namespace memory_order_sync_with_atomic {

namespace {
std::queue<int> taskQ;
std::atomic<bool> dataProduced;
std::atomic<bool> dataConsumed;
} // namespace

void dataProducer() {
    taskQ.push(10);
    dataProduced.store(true, std::memory_order::release);
}

void broker() {
    while (!dataProduced.load(std::memory_order::acquire)) {
        ;
    }
    dataConsumed.store(true, std::memory_order::release);
}

void dataConsumer() {
    while (!dataConsumed.load(std::memory_order::acquire)) {
        ;
    }
    std::cout << taskQ.front() << '\n';
    taskQ.pop();
}

void demo() {
    std::thread t1(dataConsumer);
    std::thread t2(broker);
    std::thread t3(dataProducer);

    t1.join();
    t2.join();
    t3.join();
}

} // namespace memory_order_sync_with_atomic
