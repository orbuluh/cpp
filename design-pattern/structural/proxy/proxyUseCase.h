#pragma once
#include <typeinfo>
#include <iostream>
namespace proxy {
struct IQueryDB {
    virtual int queryDB(std::string_view key) = 0;
    virtual ~IQueryDB() = default;
};

struct HeavyDBLoad : public IQueryDB {
    HeavyDBLoad() {
        std::cout << "\tHeavy DBLoad in contructor that you can't change in ctor of HeavyDBLoad\n";
    }
    ~HeavyDBLoad() override = default;
    int queryDB(std::string_view key) override {
        return 42;
    }
};

struct LazyInitProxyOfHeavyDBLoad : public IQueryDB {
    LazyInitProxyOfHeavyDBLoad() = default;
    ~LazyInitProxyOfHeavyDBLoad() override = default;

    int queryDB(std::string_view key) override {
        if (!db_) {
            std::cout << "lazy init during query!\n";
            db_ = std::make_unique<HeavyDBLoad>();
        }
        return db_->queryDB(key);
    }

    std::unique_ptr<HeavyDBLoad> db_;
};

void demo();
} // namespace proxy