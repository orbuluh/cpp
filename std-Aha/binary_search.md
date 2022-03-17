# std::lower_bound
- First element in the range [first,last) which does not compare less than val.

# std::upper_bound
- Returns an iterator pointing to the first element in the range [first,last) which compares greater than val.

# std::binary_search
- Checks if an element equivalent to value appears within the range [first, last).

```cpp
                        0   1   2   3   4   5   6   7
std::vector<int> v = { 10, 10, 10, 20, 20, 20, 30, 30 };
                                  low
                                               up
low =std::lower_bound (v.begin(), v.end(), 20);
up = std::upper_bound (v.begin(), v.end(), 20);

std::cout << "lower_bound at position " << (low- v.begin()) << '\n';  // 3
std::cout << "upper_bound at position " << (up - v.begin()) << '\n';  // 6
std::cout << "25 exist=" << std::binary_search(v.begin(), v.end(), 25) << '\n';
```

