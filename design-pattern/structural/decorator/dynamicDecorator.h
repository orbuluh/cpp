#pragma once
#include "fileDataSource.h"

namespace decorator {

struct DataSourceDecorator : public IDataSource {
    DataSourceDecorator(IDataSource* src) : wrappee_(src) {};
    ~DataSourceDecorator() override = default;
    void writeData(std::string_view rawdata) override { wrappee_->writeData(rawdata); }
    IDataSource* wrappee_;
};

struct EncryptionDecorator : public DataSourceDecorator {
    EncryptionDecorator(IDataSource* src) : DataSourceDecorator(src) {};
    ~EncryptionDecorator() override = default;
    void writeData(std::string_view rawdata) override {
        auto encryptData = std::string{rawdata.rbegin(), rawdata.rend()};
        std::cout << "Encrypt " << rawdata << " to " << encryptData << '\n';
        wrappee_->writeData(encryptData);
    }
};

struct CompressionDecorator : public DataSourceDecorator {
    CompressionDecorator(IDataSource* src) : DataSourceDecorator(src) {};
    ~CompressionDecorator() override = default;
    void writeData(std::string_view rawdata) override {
        auto compressData = std::string{rawdata.begin(), rawdata.begin() + 2};
        std::cout << "Compress " << rawdata << " to " << compressData << '\n';
        wrappee_->writeData(compressData);
    }
};

} // namespace decorator