#include "dynamicDecorator.h"
#include "staticDecorator.h"
#include "functionDecorator.h"
#include "decoratorUseCase.h"

namespace decorator {

void demoDynamicDecorator() {
    std::cout << "demo: DynamicDecorator\n";
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

void demoStaticDecorator() {
    std::cout << "demo: staticDecorator\n";
    std::cout << "--------------------\n";
    const auto fileName = "app_output.txt";
    const auto encryptFlavor = "reverseEncryption";
    const auto rawData = "ABCD";
    EncryptionStaticDecorator<FileDataSource> encryptFds(encryptFlavor, fileName);
    encryptFds.writeData(rawData);

    const auto compressFlavor = "takeFirst2Compression";
    CompressionStaticDecorator<FileDataSource> compressFds(compressFlavor, fileName);
    compressFds.writeData(rawData);

    CompressionStaticDecorator<EncryptionStaticDecorator<FileDataSource>>
        encryptAndCompressFds(compressFlavor, encryptFlavor, fileName);

    encryptAndCompressFds.writeData(rawData);
    std::cout << "--------------------\n";
}

void dummyFunction(int a, int b) {
    for (int i = 0; i < 1000; ++i) {
        a += b;
    }
}

void demo() {
    demoDynamicDecorator();
    demoStaticDecorator();
    timeTheFunctor(dummyFunction, 10, 1);
}
} // namespace decorator