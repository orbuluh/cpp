# [C++17 - The complete guide, Ch 2: if and switch with Initialization](http://www.cppstd17.com/index.html)

## 2.1 if with Initialization

- Any value initialized inside an if statement is valid until the end of the then and the else part (if this part exists).

```cpp
if (std::ofstream strm = getLogStrm(); coll.empty()) {
  strm << "<no data>\n";
} else {
  for (const auto& elem : coll) {
    strm << elem << '\n';
  }
}
// strm no longer declared - The destructor for strm is called at the end of the whole if statement (at the end of the else part, if any, or at the end of the then part, otherwise)
```

or


```cpp
if (std::lock_guard lg{collMutex}; !coll.empty()) {
  std::cout << coll.front() << '\n';
}
```

- For the guard to be effective, the initializer needs to declare a variable with a name. Otherwise, the initialization itself is **an expression that creates and immediately destroys a temporary object**.
- As a consequence, initializing a lock guard without a name is a logical error, because the guard would no longer lock when the condition is checked:

```cpp
if (std::lock_guard<std::mutex>{collMutex};  // runtime ERROR:
    !coll.empty()) {                         // - no longer locked
  std::cout << coll.front() << '\n';         // - no longer locked
}
```

You can have multiple declarations with optional initializations:

```cpp
if (auto x = qqq1(), y = qqq2(); x != y) {
  std::cout << "return values " << x << " and " << y << " differ\n";
}

// or :

if (auto x{qqq1()}, y{qqq2()}; x != y) {
  std::cout << "return values " << x << " and " << y << " differ\n";
}
```

Consider inserting a new element into a map or unordered map. You can check whether
this was successful as follows:

```cpp
std::map<std::string, int> coll;
if (auto [pos, ok] = coll.insert({"new", 42}); !ok) {
  // if insert failed, handle error using iterator pos:
  const auto& [key, val] = *pos;
  std::cout << "already there: " << key << '\n';
}
```

## 2.2 switch with Initialization

```cpp
namespace fs = std::filesystem;
switch (fs::path p{name}; status(p).type()) {
  case fs::file_type::not_found:
    std::cout << p << " not found\n";
    break;
  case fs::file_type::directory:
    std::cout << p << ":\n";
    for (const auto& e : std::filesystem::directory_iterator{p}) {
      std::cout << "- " << e.path() << '\n';
    }
    break;
  default:
    std::cout << p << " exists\n";
    break;
}
```