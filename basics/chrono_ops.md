# [`std::chrono`](https://en.cppreference.com/w/cpp/chrono/duration)

C++ includes support for two types of time manipulation:

- The chrono library, a flexible collection of types that track **time** with varying degrees of precision (e.g. `std::chrono::time_point`).
- C-style **date and time** library (e.g. `std::time`)

## `std::chrono` library

The `chrono` library defines three main types as well as utility functions and common typedefs.
- **clocks**
  - A clock consists of a **starting point** (or **epoch**) and a **tick rate**.
  - For example, a clock may have an epoch of January 1, 1970 and tick every second.
- **time points**
  - A time point is a duration of time that has passed since the epoch of a specific clock.
- **durations**
  - A duration consists of a **span of time**, defined as some **number of ticks of some time unit**.
  - For example, "42 seconds" could be represented by a duration consisting of 42 ticks of a 1-second time unit.
- **Time of day** (since C++20)
  - `hh_mm_ss` splits a duration representing time elapsed **since midnight** into hours, minutes, seconds, and fractional seconds, as applicable.
  - It is primarily a formatting tool.
  - gcc-10 not supported yet
- **Calendar** (since C++20)
- **Time zone** (since C++20)

## `std::duration` and `std::ratio`

```cpp
template<
    class Rep,  // an arithmetic type representing the number of ticks
    class Period = std::ratio<1> // a std::ratio representing the tick period
                                 // (the number of second's fractions per tick)
> class duration;
```

- The actual time interval (in seconds) that is held by a duration object d is roughly equal to `d.count() * D::period::num / D::period::den`, where `D` is of type `chrono::duration<>` and `d` is an object of such type.

where `#include <ratio>` defines:

```cpp
template<
    std::intmax_t Num,       // numerator
    std::intmax_t Denom = 1  // denominator
> class ratio;
```
- The `static` data members `num` and `den` representing the numerator and denominator are calculated by **dividing Num and Denom by their greatest common divisor.**
  - e.g. `std::ratio<3, 6>::type` is `std::ratio<1, 2>`.
- [Convenient type are defined](https://en.cppreference.com/w/cpp/numeric/ratio/ratio):
  - `nano`: `std::ratio<1, 1000000000>`
  - `micro`: `std::ratio<1, 1000000>`
  - ...
> Note that `std::nano` is a ratio, and `std::nanoseconds` is a duration!






