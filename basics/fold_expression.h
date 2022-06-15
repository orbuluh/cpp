#include <iostream>
namespace fold_expression {

template<typename ...Args>
void printer1(Args&&... args)
{

    // form 4: (I op ... op E)
    (std::cout << ... << args) << '\n';
}

template<typename ...Args>
void printer2(Args&&... args)
{
    // form 2: (... op E)
    std::cout << (... << args) << '\n';
}

void demo() {
    std::cout << "form 4: (I op ... op E)\n";
    printer1(1, 2, 3, 4);
    std::cout << "form 2: (... op E)\n";
    printer2(1, 2, 3, 4);
}

}