[Code example](vector_ops.h)

# Reverse loop

```cpp
// One way
for (auto i = my_vector.rbegin(); i != my_vector.rend(); ++i ) {
    /*do stuff */
}

// The other way
auto i = my_vector.end();
while (i != my_vector.begin()) {
    --i;
}
```

# About `erase`

- Only work for `iterator` (not `const_iterator`, `reverse_iterator`, and `const_reverse_iterator`). (Same for `insert`)
  - So if you want to reverse loop using iterator, and the erase element, you can't use reverse_iterator!!!
- Return **Iterator following the last removed element**.
  - If pos refers to the last element, then the `end()` iterator is returned.
  - If `last == end()` prior to removal, then the updated `end()` iterator is returned.
  - If [`first`, `last`) is an empty range, then `last` is returned.

# [About `erase` plus `remove_if`](https://stackoverflow.com/a/39019851/4924135)

```cpp
std::vector<int> v = {0, 1, 0, 0, 4};
// remove 0
v.erase(std::remove_if(v.begin(), v.end(), [](int e) { return e == 0; }),
        v.end());
```

- why looking like this? It's because `std::remove_if` **swaps elements inside the vector in order to put all elements that do not match the predicate towards the beginning of the container.**
- This means that **if the predicate returns true, then that element will be placed at the end of the vector**.
- `remove_if` then **returns an iterator which points to the first element which matches the predicate**. In other words, an iterator to the first element to be removed.
- `std::vector::erase` erases the range starting from the returned iterator to the end of the vector, such that all elements that match the predicate are removed.

# About `insert`

- inserts value *before* input iterator
- return Iterator pointing to *the inserted value*
