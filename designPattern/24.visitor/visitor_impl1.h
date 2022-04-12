#pragma once

#include <iostream>
#include <string>
#include <utility>
namespace visitor {
void demo();
} // namespace visitor

namespace visitor::impl1 {

// impl1 -> actual behavior defined in visitor class InsuranceSeller

// Reference: [tomas789/gist:7844152](https://gist.github.com/tomas789/7844152)

struct Residential{};
struct Bank{};
struct CoffeeShop{};

// A concrete visitor that just have all the visit(T&)
class InsuranceSeller {
public:
    // random input params to demonstrate below VisitorInitParams
    InsuranceSeller(std::string name, int id) {
        std::cout << "Seller " << name << " EmployeeId: " << id
                  << " ready to visit...\n";
    }

    // All the visit(...) with different types are here.
    void visit(Residential& r) {
        std::cout << "Sell medical insurance for residential building\n";
    }

    void visit(Bank& r) {
        std::cout << "Sell theft insurance for bank\n";
    }

    void visit(CoffeeShop& r) {
        std::cout << "Sell fire insurance for coffeeshop\n";
    }
};

template <typename... TList>
class GenericVisitor;

// base case
template <typename VisitorT, typename VisitableT>
class GenericVisitor<VisitorT, VisitableT> {
protected:
    VisitorT visitor;

public:
    template <typename... VisitorInitParams>
    GenericVisitor(VisitorInitParams&&... plist)
        : visitor(std::forward<VisitorInitParams>(plist)...) {}

    void visit(VisitableT& visitable) {
        visitor.visit(visitable);
    }
};

template <typename VisitorT, typename VisitableT, typename... TList>
class GenericVisitor<VisitorT, VisitableT, TList...>
      : public GenericVisitor<VisitorT, TList...> {
public:
    template <typename... VisitorInitParams>
    GenericVisitor(VisitorInitParams&&... plist)
        : GenericVisitor<VisitorT, TList...>(std::forward<VisitorInitParams>(plist)...) {}

    // GenericVisitor<VisitorT, A, B, C>::visit has signature visit(A&)
    // GenericVisitor<VisitorT, B, C>::visit has signature visit(B&)
    // GenericVisitor<VisitorT, C>::visit has signature visit(C&)
    // so you need to surfaces all the visit from base to derived
    using GenericVisitor<VisitorT, TList...>::visit;

    // eventually pull visitor from base
    // GenericVisitor<VisitorT, A, B, C>::visitor ==
    // GenericVisitor<VisitorT, B, C>::visitor ==
    // GenericVisitor<VisitorT, C>::visitor,
    // which hast type VisitorT in base case template
    using GenericVisitor<VisitorT, TList...>::visitor;

    // No need to be virtual
    void visit(VisitableT& visitable) { visitor.visit(visitable); }
};
} // namespace vistor::impl1