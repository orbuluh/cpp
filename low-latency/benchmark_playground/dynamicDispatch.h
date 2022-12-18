#pragma once
#include <benchmark/benchmark.h>
#include <iostream>
#include <memory>

struct BaseNpc {
    virtual ~BaseNpc() = default;
    virtual void onTalk() const = 0;
};

struct ItemSellerDd : public BaseNpc {
    ~ItemSellerDd() override = default;
    void onTalk() const override {
    }
};

struct HealerDd : public BaseNpc {
    ~HealerDd() override = default;
    void onTalk() const override {
    }
};

static void BM_DynamicDispatch(benchmark::State& state) {
    std::unique_ptr<BaseNpc> npc1 = std::make_unique<ItemSellerDd>();
    std::unique_ptr<BaseNpc> npc2 = std::make_unique<HealerDd>();
    for (auto _ : state) {
        npc1->onTalk();
        npc2->onTalk();
    }
}
BENCHMARK(BM_DynamicDispatch);

template<typename Concrete>
struct BaseNpcCrtp {
    virtual ~BaseNpcCrtp() = default;
    void onTalk() const {
        impl().talk();
    }

    const Concrete& impl() const {
        return static_cast<const Concrete&>(*this);
    }
};

struct ItemSellerSd : BaseNpcCrtp<ItemSellerSd> {
    ~ItemSellerSd() override = default;
    void talk() const {
    }
};

struct HealerSd : BaseNpcCrtp<HealerSd> {
    ~HealerSd() override = default;
    void talk() const {
    }
};

// Define another benchmark
static void BM_StaticDispatch(benchmark::State& state) {
    ItemSellerSd npc1;
    HealerSd npc2;
    for (auto _ : state) {
        npc1.onTalk();
        npc2.onTalk();
    }
}
BENCHMARK(BM_StaticDispatch);