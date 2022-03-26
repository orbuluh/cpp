#pragma once
#include <string_view>
#include <string>

namespace decorator {

struct IDataSource {
    virtual ~IDataSource() = default;
    virtual void writeData(std::string_view rawdata) = 0;
};

struct FileDataSource : public IDataSource {
    FileDataSource(std::string_view filename) : filename_(filename) {}
    ~FileDataSource() override = default;
    void writeData(std::string_view rawdata) override;
    std::string filename_;
};

struct DataSourceDecorator : public IDataSource {
    DataSourceDecorator(IDataSource* src) : wrappee_(src) {};
    ~DataSourceDecorator() override = default;
    void writeData(std::string_view rawdata) override { wrappee_->writeData(rawdata); }
    IDataSource* wrappee_;
};

struct EncryptionDecorator : public DataSourceDecorator {
    EncryptionDecorator(IDataSource* src) : DataSourceDecorator(src) {};
    ~EncryptionDecorator() override = default;
    void writeData(std::string_view rawdata) override;
};

struct CompressionDecorator : public DataSourceDecorator {
    CompressionDecorator(IDataSource* src) : DataSourceDecorator(src) {};
    ~CompressionDecorator() override = default;
    void writeData(std::string_view rawdata) override;
};

} // namespace decorator