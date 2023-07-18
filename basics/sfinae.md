# SFINAE (Substitution Failure Is Not An Error)

- SFINAE relies on the mechanism of substitution and overload resolution during template instantiation. If a substitution failure occurs in a particular template instantiation, that specific template is removed from the candidate set, and the compiler proceeds to consider other viable candidates.
- SFINAE is commonly used to **enable or disable function overloads** or **template specializations based on the properties of the template arguments**.
- It allows for **compile-time selection of the appropriate function** or **template based on the available operations or properties of the types involved**.
- SFINAE can be leveraged using techniques like `std::enable_if`, template specialization, function overloading, or type traits to achieve more flexible and customizable template-based programming.

## How do `enable_if` work?

- The `std::enable_if` type trait is typically implemented using template specialization and partial specialization.Simplified example:

```cpp
// Primary template definition
template <bool B, typename T = void>
struct enable_if {};

// Partial specialization for B=true
template <typename T>
struct enable_if<true, T> {
    using type = T;
};
```

- The primary template definition of `enable_if` serves as a fallback when the condition `B` is `false`. It takes two template parameters: a boolean value `B` and a type `T` (defaulted to `void`).
- The partial specialization of `enable_if` is triggered when `B` is `true`. It provides a nested type alias that is equal to the specified type `T`.
- When `enable_if<B, T>::type` is used, the partial specialization is chosen if `B` is true, and the primary template is chosen if `B` is `false`.
- The type `T` can be any valid C++ type, allowing flexibility in determining the resulting type based on the condition `B`.
- By using this mechanism, `std::enable_if` conditionally defines a nested type alias based on the provided boolean condition.
  - If the condition is true, the nested type alias is available, enabling the use of the resulting type in template metaprogramming scenarios.
  - If the condition is false, the type alias is not defined, leading to a substitution failure during template argument deduction.
- So how does it actually works?

```cpp
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value>::type
foo(T value) {
    // Implementation for floating-point types
}
```

- `typename std::enable_if<std::is_floating_point<T>::value>::type` basically defines the return type of the function `foo`.
- If substitution fail, the ::type won't exist, hence the template function instantiation will be ignored
- On the other hand, if the condition is true, `::type` alias exists, and the instantiation shows up!