#include "proxyUseCase.h"
#include <iostream>

namespace proxy {

void demo() {
    std::cout << "normal usage, some heavy stuff loaded during construction\n";
    HeavyDBLoad db;
    std::cout << "call query:\n" << db.queryDB("whatever") << '\n';
    std::cout << "---------------\n\n";

    std::cout << "Use proxy to delay the construction\n";
    LazyInitProxyOfHeavyDBLoad lazyDb;
    std::cout << "Done construct\n";
    std::cout << "call query:\n" << lazyDb.queryDB("whatever") << '\n';
}
} // namespace proxy