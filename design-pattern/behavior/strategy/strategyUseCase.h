#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace strategy {

enum class Boss {
    /*WebSpider,
    SplitMushroom,
    CyberPeacock,
    StormOwl,*/
    MagmaDragoon,
    FrostWalrus,
    /*JetStingray,
    SlashBeast,*/
};

const char* bossName(Boss t);

struct IWeaponStrategy {
    IWeaponStrategy(std::string name) : name_(std::move(name)) {}
    virtual ~IWeaponStrategy() = default;
    std::string_view name() const { return name_; }
    virtual int attackOn(Boss t) const = 0;

    std::string name_;
};

struct XBuster : IWeaponStrategy {
    XBuster() : IWeaponStrategy("XBuster") {}
    ~XBuster() override = default;
    int attackOn(Boss t) const override {
        return 1;
    }
};

struct RisingFire : IWeaponStrategy {
    RisingFire() : IWeaponStrategy("RisingFire") {}
    ~RisingFire() override = default;
    int attackOn(Boss t) const override {
        return (t == Boss::FrostWalrus)? 2: 1;
    }
};

struct FrostTower : IWeaponStrategy {
    FrostTower() : IWeaponStrategy("FrostTower") {}
    ~FrostTower() override = default;
    int attackOn(Boss t) const override {
        return 1;//(t == Boss::JetStingray)? 2: 1;
    }
};

class MagaManX4Context {
    IWeaponStrategy* weapon_;
public:
    MagaManX4Context() {
        weaponObtained_.emplace_back(new XBuster());
        switchWeapon();
        std::cout << "Ready Fight" << '\n';
    }

    void switchWeapon() {
        weaponIdx = (weaponIdx + 1) % weaponObtained_.size();
        weapon_ = weaponObtained_[weaponIdx].get();
    }

    void beatBoss(Boss t) {
        if (t == Boss::MagmaDragoon)
            weaponObtained_.emplace_back(new RisingFire());
        else if (t == Boss::FrostWalrus)
            weaponObtained_.emplace_back(new FrostTower());
    }

    int attack(Boss t) {
        std::cout << "Attack: " << bossName(t)
            << " using " << weapon_->name()
            << " powerMultiply=" << weapon_->attackOn(t) << "x\n";
        return weapon_->attackOn(t);
    }

    int weaponIdx = 0;
    std::vector<std::unique_ptr<IWeaponStrategy>> weaponObtained_;
};

void demo();

} // namespace strategy