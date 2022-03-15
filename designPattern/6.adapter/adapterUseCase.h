#pragma once
#include <iostream>
#include <type_traits>

namespace adapter {
struct LegacyCriticalApp {
    void doXinLegacyStyle() {
        std::cout << "do X in legacy style\n";
    }
};
struct RefactoredCriticalApp {
    void doXinRefactoredStyle() {
        std::cout << "do X in refactored style\n";
    }
};

template<typename App>
struct BackwardCompatibleAdapter {
    void doX() {
        // NOTE: you have to do compile time branching here,
        // otherwise you will get compile error like:
        // error: no member named 'doXinRefactoredStyle' in 'adapter::LegacyCriticalApp'
        //    criticalApp.doXinRefactoredStyle();
        if constexpr (std::is_same_v<App, LegacyCriticalApp>) {
            criticalApp.doXinLegacyStyle();
        } else if constexpr (std::is_same_v<App, RefactoredCriticalApp>) {
            criticalApp.doXinRefactoredStyle();
        } else {
            std::cout << "App doesn't support doing X!\n";
        }
    }
    App criticalApp;
};

void demo();
} // namespace adapter