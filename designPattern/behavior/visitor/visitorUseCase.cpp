#include "visitorUseCase.h"
#include "visitor_impl1.h"
#include "visitor_impl2.h"

namespace visitor {
void demo() {
    {
        using namespace visitor::impl1;
        Residential residential;
        Bank bank;
        CoffeeShop coffeeshop;

        GenericVisitor<InsuranceSeller,
                       Residential, Bank, CoffeeShop> visitor{"Orb", 1234};

        visitor.visit(residential);
        visitor.visit(bank);
        visitor.visit(coffeeshop);
    }
    {
        using namespace visitor::impl2;
        Residential residential;
        Bank bank;
        CoffeeShop coffeeshop;

        Customer visitor;
        visitor.visit(residential);
        visitor.visit(bank);
        visitor.visit(coffeeshop);
    }
}
} // namespace visitor
