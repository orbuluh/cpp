#include "designPattern/20.observer/observerUseCase.h"
#include "designPattern/25.maybeMonad/maybeMonadUseCase.h"
#include "boost/cookbook.h"
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
    boost_any::demo();
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    opt::options_description desc("specify -d");
    std::string demoType;
    desc.add_options()
        ("demoType,d", opt::value<std::string>(&demoType), "what demo to run")
        //("requiredArg,r", opt::value<std::string>()->required(), "how to define required field.")
        //("optionalArg,o", opt::value<std::string>()->default_value("option"), "how to define optional field.")
        ;
    opt::variables_map vm;
    try {
        opt::store(opt::parse_command_line(argc, argv, desc), vm);
    } catch (UnknownOptionException& e) {
        std::cerr << e.what() << "..." << std::endl;
    }
    opt::notify(vm);
    if (demoType == "dp") {
        runDesignPatternDemo();
    } else if (demoType == "boost") {
        runBoostDemo();
    } else if (demoType == "test") {
        return RUN_ALL_TESTS();
    }
    return 0;
}