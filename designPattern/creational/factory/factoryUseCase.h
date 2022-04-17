#pragma once
#include <memory>

namespace factory {

struct ICompresser {
    virtual ~ICompresser() = default;
    virtual void encode(char* rawdata) = 0;
    virtual void decode(char* rawdata) = 0;
};

struct Lz4Compressor : public ICompresser {
    ~Lz4Compressor() override = default;
    void encode(char* rawdata) override;
    void decode(char* rawdata) override;
};

struct Bzip2Compressor : public ICompresser {
    ~Bzip2Compressor() override = default;
    void encode(char* rawdata) override;
    void decode(char* rawdata) override;
};

enum class CompressMethod {
    Lz4,
    Bzip2,
};

class IApplication {
public:
    virtual ~IApplication() = default;
    void handleInput(char* rawdata);
    void handleOutput(char* rawdata);
protected:
    static std::unique_ptr<ICompresser> getCompresser(CompressMethod c) {
        switch (c) {
        case CompressMethod::Lz4: return std::make_unique<Lz4Compressor>();
        case CompressMethod::Bzip2: return std::make_unique<Bzip2Compressor>();
        default:
            return nullptr;
        };
    }
    std::unique_ptr<ICompresser> compressor_;
};

class Lz4App : public IApplication {
public:
    Lz4App() {
        compressor_ = getCompresser(CompressMethod::Lz4);
    }
};

class Bzip2App : public IApplication {
public:
    Bzip2App() {
        compressor_ = getCompresser(CompressMethod::Bzip2);
    }
};

void demo();
} // namespace factory