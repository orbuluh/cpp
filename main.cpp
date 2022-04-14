//#include "designPattern/4.prototype/prototypeUseCase.h" // example showned in composite::demo()
#include "designPattern/6.adapter/adapterUseCase.h"
#include "designPattern/7.bridge/bridgeUseCase.h"
#include "designPattern/8.composite/compositeUseCase.h"
#include "designPattern/9.decorator/decoratorUseCase.h"
#include "designPattern/10.facade/facadeUseCase.h"
#include "designPattern/11.flyweight/flyweightUseCase.h"
#include "designPattern/12.proxy/proxyUseCase.h"
#include "designPattern/13.chainofresp/chainofrespUseCase.h"
#include "designPattern/14.command/commandUseCase.h"
#include "designPattern/16.iterator/iteratorUseCase.h"
#include "designPattern/17.mediator/mediatorUseCase.h"
#include "designPattern/18.memento/mementoUseCase.h"
#include "designPattern/20.observer/observerUseCase.h"
#include "designPattern/21.state/stateUseCase.h"
#include "designPattern/22.strategy/strategyUseCase.h"
#include "designPattern/24.visitor/visitorUseCase.h"
#include "designPattern/25.maybeMonad/maybeMonadUseCase.h"
#include "boost/cookbook.h"

#include "std-Aha/binarySearch.h"
#include "std-Aha/vector_ops.h"

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
    //observer::demo();
    //state::demo();
    //bridge::demo();
    //adapter::demo();
    //facade::demo();
    //mediator::demo();
    //command::demo();
    //chainofresp::demo();
    //decorator::demo();
    //proxy::demo();
    //flyweight::demo();
    //memento::demo();
    //composite::demo(); // also show how prototype is used
    //iterator::demo();
    //visitor::demo();
    strategy::demo();
}

void runBoostDemo() {
    //boost_any::demo();
    boost_msm::demo();
}

void runStdDemo() {
    //std_binary_search::demo();
    std_vector_ops::demo();
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
    } else if (demoType == "std") {
        runStdDemo();
    } else if (demoType == "test") {
        return RUN_ALL_TESTS();
    }
    return 0;
}
