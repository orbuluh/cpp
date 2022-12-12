# C++ random groceries / advanced language features

## Language related

- [`static`](static.md)
- [linkage](linkage.md)
- [translation unit](translation_unit.md)
- [Comma operator](comma_operator.md)
- [Parameter pack](parameter_pack.md), [Fold expression](fold_expression.md)
- [Structural binding](structural_binding.md)
- [Special members](special_members.md)
- [`utility`](utility.md)

## `std` related

- [`std::iostream`](iostream.md)
- [`std::stringstream`](stingstream.md)
- [`std::chrono`](chrono_ops.md)
- [`std::regex`](regex.md)

### `algorithm`

- [`std::binary_search`, `std::upper_bound`, `std::lower_bound`](binary_search.md)
- [`std::next_permutation`](permutation.md)

### data structure

- [`std::vector`](vector_ops.md)
  - more about `insert` and `erase`
- [`std::list`](list_ops.md)
- [`std::priority_queue`](priority_queue.md)
- [`std::find`](find.md)
  - about `find`'s customized comparator
- [`std::is_transparent`](heterogenous_lookup.md)
  - use `std::string_view` to [unordered_]{set|map} with a `std::string` key

### Misc

- [CPU cache](cpu_cache.md)