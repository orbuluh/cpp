#pragma once
#include "../../creational/prototype/prototypeUseCase.h"
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

namespace composite {

class Box : public prototype::IGoods {
public:
    Box(std::string_view packageName) : IGoods(packageName, 0) {}
    ~Box() override = default;
    Box(Box&& box) : qtyMap_{std::move(box.qtyMap_)}
                   , goods_{std::move(box.goods_)}
                   , IGoods{box.name(), 0} {}

    void addItem(std::unique_ptr<IGoods>&& item) {
        auto name = std::string{item->name()};
        auto it = qtyMap_.find(name);
        if (it == qtyMap_.end()) {
            std::cout << "Box(" << name_ << ") adding item: " << name
                << " for price: " << item->value() << '\n';
            goods_.emplace_back(std::move(item));
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