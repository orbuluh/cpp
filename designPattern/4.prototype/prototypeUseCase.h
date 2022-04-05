#pragma once
#include <map>
#include <memory>
#include <mutex>
#include <string_view>
namespace prototype {

class IGoods {
public:
    IGoods(std::string_view name, float price) : name_(name), price_(price) {}
    virtual ~IGoods() = default;
    virtual float value() { return price_; };
    std::unique_ptr<IGoods> clone() {
        return std::make_unique<IGoods>(name_, price_);
    }
    std::string_view name() { return name_; }
protected:
    std::string name_;
    float price_;
};

enum class Item {
    NikePg5EpSonyPS5Edition,
    NikeBasketBallSocks,
    NikeWaffleDebut,
};

class GoodsFactory {
public:
    static std::unique_ptr<IGoods> create(Item itemEnum) {
        static std::map<Item, std::unique_ptr<IGoods>> products;
        std::call_once(initialized, [](){
            products[Item::NikePg5EpSonyPS5Edition] = std::move(std::make_unique<IGoods>("NikePg5EpSonyPS5Edition", 1357.0));
            products[Item::NikeBasketBallSocks] = std::move(std::make_unique<IGoods>("NikeBasketBallSocks", 79.0));
            products[Item::NikeWaffleDebut] = std::move(std::make_unique<IGoods>("NikeWaffleDebut", 599.0));
        });
        return products[itemEnum]->clone();
    }

    static std::once_flag initialized;
};


// check composite::demo() for the usage

} // namespace prototype