#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

namespace composite {

class IGoods {
public:
    IGoods(std::string_view name, float price) : name_(name), price_(price) {}
    virtual ~IGoods() = default;
    virtual float value() { return price_; };
    std::string_view name() { return name_; }
protected:
    std::string name_;
    float price_;
};

class Box : public IGoods {
public:
    Box(std::string_view packageName) : IGoods(packageName, 0) {}
    ~Box() override = default;
    Box(Box&& box) : qtyMap_{std::move(box.qtyMap_)}
                   , goods_{std::move(box.goods_)}
                   , IGoods{box.name(), 0} {}

    void addItem(std::string name, float price) {
        auto it = qtyMap_.find(name);
        if (it == qtyMap_.end()) {
            std::cout << "Box(" << name_ << ") adding item: " << name
                << " for price: " << price << '\n';
            goods_.emplace_back(std::make_unique<IGoods>(name, price));
        }
        qtyMap_[name] += 1;
    }
    void addBox(std::unique_ptr<Box>&& box) {
        std::cout << "Box(" << name_ << ") adding box: " << box->name() << '\n';
        qtyMap_[std::string{box->name()}] += 1;
        goods_.emplace_back(std::move(box));
    }
    float value() override {
        return std::accumulate(goods_.begin(), goods_.end(), 0.0,
            [&](auto sum, const auto& item) {
                const auto qty = this->qtyMap_[std::string{item->name()}];
                std::cout << "\tBox(" << name_ << "): " << item->name() << " qty=" << qty << '\n';
                return sum + item->value() * qty;
            });
    }
private:
    std::map<std::string, int> qtyMap_;
    std::vector<std::unique_ptr<IGoods>> goods_;
};

void demo();
} // namespace composite