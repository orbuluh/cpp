#include "factoryUseCase.h"
#include <iostream>

namespace factory {

void Lz4Compressor::encode(char* rawdata) {
    std::cout << "encode rawdata with lz4\n";
}
void Lz4Compressor::decode(char* rawdata) {
    std::cout << "decode rawdata with lz4\n";
}

void Bzip2Compressor::encode(char* rawdata) {
    std::cout << "encode rawdata with bzip2\n";
}

void Bzip2Compressor::decode(char* rawdata) {
    std::cout << "decode rawdata with bzip2\n";
}

void IApplication::handleInput(char* rawdata)
{
    compressor_->decode(rawdata);
}

void IApplication::handleOutput(char* rawdata)
{
    compressor_->encode(rawdata);
}

void demo() {
    Lz4App lz4app;
    lz4app.handleInput("abc");
    lz4app.handleOutput("abc");
    Bzip2App bzip2app;
    bzip2app.handleInput("abc");
    bzip2app.handleOutput("abc");
}
} // namespace factory