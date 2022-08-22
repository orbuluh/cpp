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

namespace memory_fence_producer_consumer {

namespace {
int nonAtomicInt = 0;
std::atomic<int> atomicInt = 0;
std::atomic<std::string*> atomicStrPtr;
} // namespace

void producer() {
    std::string* p = new std::string("test");
    nonAtomicInt = 1234;
    atomicInt.store(4321, std::memory_order_relaxed);
    // the write before the fence wno't be reordered, so these 2 int are also
    // guarantee to be valid given we put a acquired fence before printing out
    std::atomic_thread_fence(std::memory_order_release);
    // intentionally put a relaxed write after the fence
    // it's fine in consumer because we got the fence
    atomicStrPtr.store(p, std::memory_order_relaxed);
}

void consumer() {
    std::string* p = nullptr;
    while (!(p = atomicStrPtr.load(std::memory_order_relaxed))) {
        // the read here won't be reordered after the fence
        // so p is guarantee to be valid, even with the relaxed operation
        ;
    }
    // sync with fence ..release, the read before can't be reorder after the fence
    std::atomic_thread_fence(std::memory_order_acquire);
    std::cout << nonAtomicInt << '\n';
    std::cout << atomicInt << '\n';
    std::cout << *p << '\n'; // guarantee to be valid
    delete p;
}

void demo() {
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join();
    t2.join();
}

} // namespace memory_fence_producer_consumer

namespace memory_order_relaxed_counter {

namespace {
int nonAtomicCounter = 0;
std::atomic<int> atomicCounter = 0;
} // namespace

void write() {
    for (int i = 0; i < 1000; ++i) {
        nonAtomicCounter++;
        atomicCounter.fetch_add(1, std::memory_order_relaxed);
    }
}

void demo() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(write);
    }
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "nonAtomicCounter=" << nonAtomicCounter << '\n';
    std::cout << "atomicCounter=" << atomicCounter << '\n';
}

} // namespace memory_order_relaxed_counter

