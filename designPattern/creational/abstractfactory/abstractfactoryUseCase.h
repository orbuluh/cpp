#pragma once
#include <iostream>
#include <memory>


namespace abstractfactory {

struct IJumpShot {
    virtual ~IJumpShot() = default;
    virtual void shot() = 0;
};
struct ICrossOver {
    virtual ~ICrossOver() = default;
    virtual void cross() = 0;
};

struct IAnimationFactory {
    virtual ~IAnimationFactory() = default;
    virtual std::unique_ptr<IJumpShot> createJumpShot() const = 0;
    virtual std::unique_ptr<ICrossOver> createCrossOver() const = 0;
};

// -------------------------

struct ExperimentalJumpshot : public IJumpShot {
    ~ExperimentalJumpshot() override = default;
    void shot() override {
        std::cout << "experimental shot!\n";
    }
};

struct ExperimentalCrossOver : public ICrossOver {
    ~ExperimentalCrossOver() override = default;
    void cross() override {
        std::cout << "experimental cross!\n";
    }
};

struct ExperimentalAnimation : public IAnimationFactory {
    ~ExperimentalAnimation() override = default;
    std::unique_ptr<IJumpShot> createJumpShot() const override {
        return std::make_unique<ExperimentalJumpshot>();
    }
    std::unique_ptr<ICrossOver> createCrossOver() const override {
        return std::make_unique<ExperimentalCrossOver>();
    }
};

// -------------------------

struct ProductionJumpshot : public IJumpShot {
    ~ProductionJumpshot() override = default;
    void shot() override {
        std::cout << "production shot!\n";
    }
};

struct ProductionCrossOver : public ICrossOver {
    ~ProductionCrossOver() override = default;
    void cross() override {
        std::cout << "production cross!\n";
    }
};

struct ProductionAnimation : public IAnimationFactory {
    ~ProductionAnimation() override = default;
    std::unique_ptr<IJumpShot> createJumpShot() const override {
        return std::make_unique<ProductionJumpshot>();
    }
    std::unique_ptr<ICrossOver> createCrossOver() const override {
        return std::make_unique<ProductionCrossOver>();
    }
};

// -------------------------

struct NbaPlayer {
    // dependency injection (DI) with abstract factory
    NbaPlayer(const IAnimationFactory& iaf) :
        jumpshot_(std::move(iaf.createJumpShot())),
        crossover_(std::move(iaf.createCrossOver()))
    {}
    void play() {
        jumpshot_->shot();
        crossover_->cross();
    }
    std::unique_ptr<IJumpShot> jumpshot_;
    std::unique_ptr<ICrossOver> crossover_;
};

void demo();
} // namespace abstractfactory