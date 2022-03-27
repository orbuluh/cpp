#pragma once

#include "fileDataSource.h"

namespace decorator {

template <typename WrappeeT>
struct EncryptionStaticDecorator : public WrappeeT {
    static_assert(std::is_base_of_v<IDataSource, WrappeeT>, "Template argument must be a IDataSource");
    template<typename... Args>
    EncryptionStaticDecorator(std::string_view encryptFlavor, Args ...args) :
        WrappeeT(std::forward<Args>(args)...), encryptFlavor_(encryptFlavor) {}
    ~EncryptionStaticDecorator() override = default;
    void writeData(std::string_view rawdata) override {
        auto encryptData = std::string{rawdata.rbegin(), rawdata.rend()};
        std::cout << "Encrypt " << rawdata << " to " << encryptData << " using " << encryptFlavor_ << '\n';
        WrappeeT::writeData(rawdata);
    }
    std::string encryptFlavor_;
};

template <typename WrappeeT>
struct CompressionStaticDecorator : public WrappeeT {
    static_assert(std::is_base_of_v<IDataSource, WrappeeT>, "Template argument must be a IDataSource");
    template<typename... Args>
    CompressionStaticDecorator(std::string_view compressFlavor, Args ...args) :
        WrappeeT(std::forward<Args>(args)...), compressFlavor_(compressFlavor) {}
    ~CompressionStaticDecorator() override = default;
    void writeData(std::string_view rawdata) override {
        auto compressData = std::string{rawdata.begin(), rawdata.begin() + 2};
        std::cout << "Compress " << rawdata << " to " << compressData << " using " << compressFlavor_ << '\n';
        WrappeeT::writeData(rawdata);
    }
    std::string compressFlavor_;
};

} // namespace decorator