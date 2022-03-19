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
- Return Iterator following the last removed element.
    - If pos refers to the last element, then the end() iterator is returned.
    - If last == end() prior to removal, then the updated end() iterator is returned.
    - If [first, last) is an empty range, then last is returned.

# About `insert`
- inserts value *before* input iterator
- return Iterator pointing to *the inserted value*