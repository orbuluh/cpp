#include "cookbook.h"

#include <boost/program_options.hpp>
#include <iostream>

namespace opt = boost::program_options;
using UnknownOptionException = boost::wrapexcept<boost::program_options::unknown_option>;

void runBoostDemo() {
    boost_asio::demo();
    //boost_any::demo();
    //boost_msm::demo();
}

int main(int argc, char* argv[]) {
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
    //if (demoType == "std")
    runBoostDemo();
    return 0;
}
