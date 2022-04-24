# Quick fact
- `std::find` has signature like: `iterator find( const Key& key );`
- as `std::string` can't be constructed from `std::string_view` implicitly, you can't use `std::string_view` to search a set/map with key type being `std::string` directly.
- C++14 fills the gap, by providing new overloads of the `find` method (along with new overloads of `count`, `lower_bound`, `upper_bound` and `equal_range`.
  - To activate these overloads, the comparison function object has to define a `typedef` called `is_transparent`.
  - The value of this typedef is not used so it doesn’t matter what it is equal to, as long as it is defined.
- check [example](heterogenous_lookup.h)

# Reference
- [std::unordered_map::find using a type different than the Key type?](https://stackoverflow.com/a/64101153/4924135)
- [is_transparent: How to search a C++ set with another type than its key](https://www.fluentcpp.com/2017/06/09/search-set-another-type-key/)