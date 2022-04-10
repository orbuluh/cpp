#pragma once
#include "visitor.h"
#include <iostream>
#include <string>
namespace visitor {

struct Residential;
struct Bank;
struct CoffeeShop;

class BuildingVisitor : public Visitor<Residential, Bank, CoffeeShop>
{};


class Residential : VisitableElement<Residential, Bank, CoffeeShop> {
public:
    void accept(BuildingVisitor& residential) override {
        std::cout << "Sell medical insurance\n";
    }
};

class Bank : VisitableElement<Residential, Bank, CoffeeShop> {
public:
    void accept(BuildingVisitor& bank) override {
        std::cout << "Sell theft insurance\n";
    }
};

class CoffeeShop : VisitableElement<Residential, Bank, CoffeeShop> {
public:
    void accept(BuildingVisitor& coffeeshop) override {
        std::cout << "Sell fire insurance\n";
    }
};

struct InsuranceSeller
       : ElementImpl<InsuranceSeller,
                     Residential, Bank, CoffeeShop> {};


void demo();
} // namespace visitor