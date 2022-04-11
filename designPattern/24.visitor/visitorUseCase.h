#pragma once

#include "visitor.h"
#include <iostream>
#include <string>
#include <utility>
namespace visitor {

// Reference: [tomas789/gist:7844152](https://gist.github.com/tomas789/7844152)

struct Residential;
struct Bank;
struct CoffeeShop;

struct Residential : VisitableImpl<Residential,
                                   Residential, Bank, CoffeeShop> {};

struct Bank : VisitableImpl<Bank,
                            Residential, Bank, CoffeeShop> {};

struct CoffeeShop : VisitableImpl<CoffeeShop,
                                  Residential, Bank, CoffeeShop> {};

class InsuranceSeller {
public:
    // random input params to demonstrate
    InsuranceSeller(std::string name, int id) {
        std::cout << "Seller " << name << " EmployeeId: " << id
                  << " ready to visit...\n";
    }
    void visit(Residential& r) {
        std::cout << "Sell medical insurance\n";
    }

    void visit(Bank& r) {
        std::cout << "Sell theft insurance\n";
    }

    void visit(CoffeeShop& r) {
        std::cout << "Sell fire insurance\n";
    }
};

template <typename... TList>
class GenericVisitor;

template <typename VisitorT, typename VisitableT>
class GenericVisitor<VisitorT, VisitableT> {
protected:
    VisitorT visitor;

public:
    template <typename... VisitorInitParams>
    GenericVisitor(VisitorInitParams&&... plist)
        : visitor(std::forward<VisitorInitParams>(plist)...) {}

    virtual void visit(VisitableT& visitable) { visitor.visit(visitable); }
};

template <typename VisitorT, typename VisitableT, typename... TList>
class GenericVisitor<VisitorT, VisitableT, TList...>
      : public GenericVisitor<VisitorT, TList...> {
public:
    template <typename... VisitorInitParams>
    GenericVisitor(VisitorInitParams&&... plist)
        : GenericVisitor<VisitorT, TList...>(std::forward<VisitorInitParams>(plist)...) {}

    using GenericVisitor<VisitorT, TList...>::visit;
    using GenericVisitor<VisitorT, TList...>::visitor;

    virtual void visit(VisitableT& visitable) { visitor.visit(visitable); }
};

void demo();
} // namespace visitor