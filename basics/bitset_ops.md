# [`std::bitset<N>`](https://en.cppreference.com/w/cpp/utility/bitset)

- The class template bitset represents a fixed-size sequence of N bits.
- Bitsets can be manipulated by standard logic operators and converted **to and from strings and integers**.
- For the purpose of the string representation and of naming directions for shift operations, the sequence is thought of as **having its lowest indexed elements at the right**, as in the binary representation of integers.

# Access

- `test(idx)`
- `all()`
- `any()`
- `none()`
- `count()`

# Modifiers
- `set(idx)`, `reset(idx)`, `flip(idx)`

# Logical operation

- `&`, `|`, `^`, `~`
- `<<`, `>>`

# Conversion
- Note: `std::overflow_error` if the value can not be represented
- `to_string`
- `to_ulong`
- `to_ullong`

# [sizeof bitset?](https://stackoverflow.com/questions/12459563/what-is-the-size-of-bitset-in-c)
- It's implementation defined, look at bitset header of your compiler.
- Potentially: 64-bit system: `8 * ((N + 63) / 64)`, e.g. align to every 8 bytes