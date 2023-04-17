# `std::vector`

[Code example](vector_ops.h)

## Reverse loop

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

## About `erase`

- Only work for `iterator` (not `const_iterator`, `reverse_iterator`, and `const_reverse_iterator`). (Same for `insert`)
  - So if you want to reverse loop using iterator, and the erase element, you can't use reverse_iterator!!!
- Return **Iterator following the last removed element**.
  - If pos refers to the last element, then the `end()` iterator is returned.
  - If `last == end()` prior to removal, then the updated `end()` iterator is returned.
  - If [`first`, `last`) is an empty range, then `last` is returned.

## [About `erase` plus `remove_if`](https://stackoverflow.com/a/39019851/4924135)

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

## About `insert`

- inserts value *before* input iterator
- return Iterator pointing to *the inserted value*

## [About `emplace_back(Type&& _Val)` and `push_back(Type&& _Val)`](https://stackoverflow.com/a/4306581/4924135)

- The function `emplace_back(Type&& _Val)` provided by MSCV10 is non conforming and redundant, because as you noted it is strictly equivalent to `push_back(Type&& _Val)`.

- But the real C++0x form of `emplace_back` is really useful: `void emplace_back(Args&&...);`

- Instead of taking a `value_type` it takes **a variadic list of arguments**, so that means that **you can now perfectly forward the arguments and construct directly an object into a container without a temporary at all**.
- That's useful because no matter how much cleverness RVO and move semantic bring to the table t**here is still complicated cases where a `push_back` is likely to make unnecessary copies (or move).**