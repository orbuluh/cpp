#include "designPattern/20.observer/observerUseCase.h"
#include "designPattern/25.maybeMonad/maybeMonadUseCase.h"

#include <boost/program_options.hpp>

#include <gtest/gtest.h>
#include <iostream>

namespace opt = boost::program_options;
using UnknownOptionException = boost::wrapexcept<boost::program_options::unknown_option>;

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}

void runDesignPatternDemo() {
    //maybemonad::demo();
    observer::demo();
}

void runBoostDemo() {

}

void runAllTest() {

}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    opt::options_description desc("...");
    std::string runMode;
    desc.add_options()
        ("demoType,d", opt::value<std::string>(&runMode), "what demo to run");
    opt::variables_map vm;
    try {
        opt::store(opt::parse_command_line(argc, argv, desc), vm);
    } catch (UnknownOptionException& e) {
        std::cerr << e.what() << "... ignored" << std::endl;
    }
    opt::notify(vm);
    if (vm.count("help")) {
        std::cout << desc << "\n"; return 1;
    }
    if (runMode == "dp") {
        runDesignPatternDemo();
    } else if (runMode == "boost") {
        runBoostDemo();
    } else if (runMode == "test") {
        return RUN_ALL_TESTS();
    }
    return 0;
}
