#pragma once
#include <string_view>
#include <string>
#include <iostream>

namespace decorator {

struct IDataSource {
    virtual ~IDataSource() = default;
    virtual void writeData(std::string_view rawdata) = 0;
};

struct FileDataSource : public IDataSource {
    FileDataSource(std::string_view filename) : filename_(filename) {}
    ~FileDataSource() override = default;
    void writeData(std::string_view rawdata) override {
        std::cout << "FileDataSource write: " << rawdata << " to " << filename_ << '\n';
    }
    std::string filename_;
};

} // namespace decorator