#pragma once
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace heterogenous_lookup {

struct string_equal {
    using is_transparent = std::true_type; // or you can simply = void
    bool operator()(std::string_view key1, std::string_view key2) const noexcept {
        return key1 == key2;
    }
};

struct string_hash {
    using is_transparent = std::true_type; // or you can simply = void
    bool operator()(std::string_view key) const noexcept {
        return std::hash<std::string_view>()(key);
    }
};

struct NameMap {
    void add(std::string_view key, int val) {
        userMap_.emplace(key, val);
    }

    int getWithCopy(std::string_view key) const {
        // T& std::unordered_map::at( const Key& key );
        // and string can't be emplicit constructed from string_view
        // so you have to be explicit transform here
        // (same situation for operator[])
        return userMap_.at(std::string{key});
    }

    int get(std::string_view key) const {
        // std::is_transparent enable find std::string with std::string_view
        const auto it = userMap_.find(key);
        return it == userMap_.end()? 0: it->second;
    }

    std::unordered_map<std::string, int, string_hash, string_equal> userMap_;
};

struct NameSet {
    void add(std::string_view key) {
        userSet_.emplace(key);
    }

    bool contains(std::string_view key) const{
        // std::is_transparent enable count std::string with std::string_view
        return userSet_.count(key) != 0;
    }
    std::unordered_set<std::string, string_hash, string_equal> userSet_;
};

void demo() {
    NameMap m;
    m.add("gg", 2);
    std::cout << m.get("gg") << '\n';
    NameSet s;
    s.add("gg");
    std::cout << s.contains("gg") << '\n';
}

} // namespace heterogenous_lookup