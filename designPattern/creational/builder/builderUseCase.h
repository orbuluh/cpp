#pragma once
#include <string>
#include <string_view>
#include <vector>
namespace builder {

// Modified/extract partial functionalities as demo from https://github.com/six-ddc/sql-builder/blob/master/sql.h

template <typename T>
inline std::string as_str(const T& data) {
    return std::to_string(data);
}

template <>
inline std::string as_str<std::string>(const std::string& data) {
    std::string str("'");
    str.append(data);
    str.append("'");
    return str;
}

class column {
public:
    column(std::string_view col) {
        cond_ = col;
    }
    std::string_view str() const { return cond_; }
public:
    column& is_not_null() {
        cond_.append(" is not null");
        return *this;
    }
    column& operator&&(column& condition) {
        std::string res("(");
        res.append(cond_);
        res.append(") and (");
        res.append(condition.cond_);
        res.append(")");
        condition.cond_ = std::move(res);
        return condition;
    }
    template <typename T>
    column& operator>=(const T& data) {
        cond_.append(" >= ");
        cond_.append(as_str(data));
        return *this;
    }
protected:
    std::string cond_;
};

class ISqlQueryBuilder {
public:
    virtual ~ISqlQueryBuilder() = default;
    virtual std::string_view str() = 0;
    std::string_view queryStr() { return sql_; }
protected:
    std::string sql_;
protected:
    template<typename T>
    void join_vector(std::string& res, const std::vector<T>& vec, std::string_view sep) {
        const auto sz = vec.size();
        for (size_t i = 0; i < sz; ++i) {
            res.append(vec[i]);
            if (i != sz - 1)
                res.append(sep);
        }
    }
};

class SelectBuilder : public ISqlQueryBuilder {
public:
    ~SelectBuilder() override = default;
    std::string_view str() override {
        sql_.clear();
        sql_.append("select ");
        if (distinct_) {
            sql_.append("distinct ");
        }
        join_vector(sql_, selectColumns_, ", ");
        sql_.append(" from ");
        sql_.append(tableName_);
        if (!whereConditions_.empty()) {
            sql_.append(" where ");
            join_vector(sql_, whereConditions_, ", ");
        }
        if(!orderBy_.empty()) {
            sql_.append(" order by ");
            sql_.append(orderBy_);
        }
        return sql_;
    }

    SelectBuilder& reset() {
        distinct_ = false;
        selectColumns_.clear();
        tableName_.clear();
        whereConditions_.clear();
        orderBy_.clear();
        return *this;
    }
public:
    template <typename... Args>
    SelectBuilder& select(std::string_view str, Args&&... columns) {
        selectColumns_.emplace_back(str);
        select(columns...);
        return *this;
    }
    // base case for templated select(...)
    SelectBuilder& select() { return *this; }
public:
    template <typename... Args>
    SelectBuilder& from(std::string_view tableName, Args&&... tables) {
        if (tableName_.empty()) {
            tableName_ = tableName;
        } else {
            tableName_.append(", ");
            tableName_.append(tableName);
        }
        from(tables...);
        return *this;
    }
    // base case for templated from(...)
    SelectBuilder& from() { return *this; }
public:
    SelectBuilder& distinct() {
        distinct_ = true;
        return *this;
    }
public:
    SelectBuilder& where(std::string_view condition) {
        whereConditions_.emplace_back(condition);
        return *this;
    }
    SelectBuilder& where(column condition) {
        whereConditions_.emplace_back(condition.str());
        return *this;
    }
public:
    SelectBuilder& order_by(std::string_view order_by) {
        orderBy_ = order_by;
        return *this;
    }

protected:
    bool distinct_ = false;
    std::vector<std::string> selectColumns_;
    std::string tableName_;
    std::vector<std::string> whereConditions_;
    std::string orderBy_;
};

void demo();
} // namespace builder