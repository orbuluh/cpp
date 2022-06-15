#include <iostream>
namespace fold_expression {

template<typename ...Args>
void form4print(Args&&... args)
{
    // form 4: (I op ... op E)
    (std::cout << ... << args) << '\n';
}

template<typename ...Args>
void form2print(Args&&... args)
{
    // form 2: (... op E)
    std::cout << (... << args) << '\n';
}

template<typename ...Args>
void form1print(Args&&... args) // MAGIC!!!
{
    // form 1: ( pack op ... )
    auto printWithNewLine
        = [](const auto& x) { std::cout << x << '\n'; };
    (printWithNewLine(args), ...);
}

void demo() {
    std::cout << "form 4: (I op ... op E)\n";
    form4print(1, 2, 3, 4);
    std::cout << "form 2: (... op E)\n";
    form2print(1, 2, 3, 4);
    std::cout << "form 1: ( pack op ... )\n";
    form1print(1, 2, 3, 4);
}

}