# Per: Performance
- These are rules that relate to how to use as little time and as few resources as possible to achieve a task in a predictably short time.
- The rules in this section are more restrictive and intrusive than what is needed for many (most) applications.
- Do not naïvely try to follow them in general code: achieving the goals of low latency requires extra work.


## Per.1: Don't optimize without reason
- If there is no need for optimization, the main result of the effort will be more errors and higher maintenance costs.


## Per.2: Don't optimize prematurely
- Elaborately optimized code is usually larger and harder to change than unoptimized code.


## Per.3: Don't optimize something that's not performance critical
- Optimizing a non-performance-critical part of a program has no effect on system performance.
- If your program spends most of its time waiting for the web or for a human, optimization of in-memory computation is probably useless.
- Put another way: If your program spends 4% of its processing time doing computation A and 40% of its time doing computation B, a 50% improvement on A is only as impactful as a 5% improvement on B.
  - (If you don't even know how much time is spent on A or B, see [Per.1](#per1-dont-optimize-without-reason) and [Per.2](#per2-dont-optimize-prematurely).)

## Per.4: Don't assume that complicated code is necessarily faster than simple code
- Simple code can be very fast. Optimizers sometimes do marvels with simple code
```cpp
// Example, good
// clear expression of intent, fast execution

vector<uint8_t> v(100000);

for (auto& c : v)
    c = ~c;
```
```cpp
// Example, bad
// intended to be faster, but is often slower

vector<uint8_t> v(100000);

for (size_t i = 0; i < v.size(); i += sizeof(uint64_t)) {
    uint64_t& quad_word = *reinterpret_cast<uint64_t*>(&v[i]);
    quad_word = ~quad_word;
}
```

## Per.5: Don't assume that low-level code is necessarily faster than high-level code
- Low-level code sometimes inhibits optimizations. Optimizers sometimes do marvels with high-level code.

## Per.6: Don't make claims about performance without measurements
- The field of performance is littered with myth and bogus folklore. Modern hardware and optimizers defy naive assumptions; even experts are regularly surprised.
- Note: Getting good performance measurements can be hard and require specialized tools.
- A few simple microbenchmarks using Unix time or the standard-library <chrono> can help dispel the most obvious myths.
- If you can't measure your complete system accurately, at least try to measure a few of your key operations and algorithms. A profiler can help tell you which parts of your system are performance critical. Often, you will be surprised.


## Per.7: Design to enable optimization
- Because we often need to optimize the initial design. Because a design that ignores the possibility of later improvement is hard to change.

- Example: From the C (and C++) standard: `void qsort(void* base, size_t num, size_t size, int (*compar)(const void*, const void*));`
  - When did you even want to sort memory? Really, we sort sequences of elements, typically stored in containers.
  - A call to qsort throws away much useful information (e.g., the element type), forces the user to repeat information already known (e.g., the element size), and forces the user to write extra code (e.g., a function to compare doubles).
  - This implies added work for the programmer, is error-prone, and deprives the compiler of information needed for optimization.

```cpp
double data[100];
// ... fill a ...

// 100 chunks of memory of sizeof(double) starting at
// address data using the order defined by compare_doubles
qsort(data, 100, sizeof(double), compare_doubles);
```
- From the point of view of interface design, `qsort` throws away useful information.
- We can do better (in C++98)
```cpp
template<typename Iter>
    void sort(Iter b, Iter e);  // sort [b:e)

sort(data, data + 100);
```
- Here, we use the compiler's knowledge about the size of the array, the type of elements, and how to compare doubles.
- With C++20, we can do better still

```cpp
// sortable specifies that c must be a
// random-access sequence of elements comparable with <
void sort(sortable auto& c);
sort(c);
```
- The key is to pass sufficient information for a good implementation to be chosen. In this, the sort interfaces shown here still have a weakness: They implicitly rely on the element type having less-than (<) defined.
- To complete the interface, we need a second version that accepts a comparison criteria:
```
// compare elements of c using p
template<random_access_range R, class C> requires sortable<R, C>
void sort(R&& r, C c);
```
- The standard-library specification of sort offers those two versions, and more.

- Note: Premature optimization is said to be the root of all evil, but that's not a reason to despise performance.
  - **It is never premature to consider what makes a design amenable to improvement,** and improved performance is a commonly desired improvement.
- **Aim to build a set of habits that by default results in efficient, maintainable, and optimizable code**.
- In particular, when you write a function that is not a one-off implementation detail, consider
  - **Information passing**:
    - Prefer clean interfaces carrying sufficient information for later improvement of implementation.
    - Note that information flows into and out of an implementation through the interfaces we provide.
  - **Compact data**:
    - By default, use compact data, such as `std::vector` and access it in a systematic fashion.
    - If you think you need a linked structure, try to craft the interface so that this structure isn't seen by users.
  - **Function argument passing and return**:
    - Distinguish between mutable and non-mutable data.
    - Don't impose a resource management burden on your users.
    - Don't impose spurious run-time indirections on your users.
    - Use conventional ways of passing information through an interface;
    - unconventional and/or "optimized" ways of passing data can seriously complicate later reimplementation.
  - **Abstraction**:
    - Don't overgeneralize;
      - a design that tries to cater for every possible use (and misuse) and
      - defers every design decision for later (using compile-time or run-time indirections) is usually a complicated, bloated, hard-to-understand mess.
    - Generalize from concrete examples, preserving performance as we generalize.
    - Do not generalize based on mere speculation about future needs.
    - The ideal is zero-overhead generalization.
  - **Libraries**:
    - Use libraries with good interfaces.
    - If no library is available build one yourself and imitate the interface style from a good library.
    - The standard library is a good first place to look for inspiration.
  - **Isolation**:
    - Isolate your code from messy and/or old-style code by providing an interface of your choosing to it.
    - This is sometimes called "providing a wrapper" for the useful/necessary but messy code.
    - Don't let bad designs "bleed into" your code.
```cpp
template<class ForwardIterator, class T>
bool binary_search(ForwardIterator first, ForwardIterator last, const T& val);
binary_search(begin(c), end(c), 7); // will tell you whether 7 is in c or not.
                                   // However, it will not tell you where that 7 is
                                   // or whether there are more than one 7.
```
- Sometimes, just passing the minimal amount of information back (here, true or false) is sufficient,
- but a good interface passes needed information back to the caller.
- Therefore, the standard library also offers
```cpp
template<class ForwardIterator, class T>
ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& val);
```
- `lower_bound` returns an iterator to the first match if any, otherwise to the first element greater than val, or last if no such element is found.
- However, lower_bound still doesn't return enough information for all uses, so the standard library also offers
```cpp
template<class ForwardIterator, class T>
pair<ForwardIterator, ForwardIterator>
equal_range(ForwardIterator first, ForwardIterator last, const T& val);
```
- `equal_range` returns a pair of iterators specifying the first and one beyond last match.
```cpp
auto r = equal_range(begin(c), end(c), 7);
for (auto p = r.first; p != r.second; ++p)
    cout << *p << '\n';
```
- Obviously, these three interfaces are implemented by the same basic code.
- They are simply three ways of presenting the basic binary search algorithm to users,
- ranging from the simplest ("make simple things simple!") to returning complete, but not always needed, information ("don't hide useful information").
- Naturally, crafting such a set of interfaces requires experience and domain knowledge.
- Note: Do not simply craft the interface to match the first implementation and the first use case you think of.
- Once your first initial implementation is complete, review it; once you deploy it, mistakes will be hard to remedy.

- Note: A need for efficiency does not imply a need for low-level code. High-level code does not imply slow or bloated.
- Note: Things have costs. Don't be paranoid about costs (modern computers really are very fast), but have a rough idea of the order of magnitude of cost of what you use.
- For example, have a rough idea of the cost of a memory access, a function call, a string comparison, a system call, a disk access, and a message through a network.
- Note: If you can only think of one implementation, you probably don't have something for which you can devise a stable interface.
- Maybe, it is just an implementation detail - not every piece of code needs a stable interface - but pause and consider.
- One question that can be useful is "what interface would be needed if this operation should be implemented using multiple threads? be vectorized?"

- Note: This rule does not contradict the Don't optimize prematurely rule.
- It complements it encouraging developers enable later - appropriate and non-premature - optimization, if and where needed.


## Per.10: Rely on the static type system
- Type violations, weak types (e.g. void*s), and low-level code (e.g., manipulation of sequences as individual bytes) make the job of the optimizer much harder.
- Simple code often optimizes better than hand-crafted complex code.
