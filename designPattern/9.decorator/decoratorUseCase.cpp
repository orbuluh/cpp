#include "decoratorUseCase.h"
#include <iostream>

namespace decorator {

void FileDataSource::writeData(std::string_view rawdata) {
    std::cout << "FileDataSource write: " << rawdata << " to " << filename_ << '\n';
}

void EncryptionDecorator::writeData(std::string_view rawdata) {
    auto encryptData = std::string{rawdata.rbegin(), rawdata.rend()};
    std::cout << "Encrypt " << rawdata << " to " << encryptData << '\n';
    wrappee_->writeData(encryptData);
}

void CompressionDecorator::writeData(std::string_view rawdata) {
    auto compressData = std::string{rawdata.begin(), rawdata.begin() + 2};
    std::cout << "Compress " << rawdata << " to " << compressData << '\n';
    wrappee_->writeData(compressData);
}

void demoDynamicDecorator() {
    std::cout << "demo: DynamicDecoratorn\n";
    std::cout << "--------------------\n";
    FileDataSource fds("app_output.txt");
    const auto rawData = "ABCD";
    fds.writeData(rawData);
    EncryptionDecorator encryptFds(static_cast<IDataSource*>(&fds));
    encryptFds.writeData(rawData);

    CompressionDecorator compressFds(static_cast<IDataSource*>(&fds));
    compressFds.writeData(rawData);

    CompressionDecorator encryptAndCompressFds(static_cast<IDataSource*>(&encryptFds));
    encryptAndCompressFds.writeData(rawData);
    std::cout << "--------------------\n";
}

void demo() {
    demoDynamicDecorator();
}
} // namespace decorator