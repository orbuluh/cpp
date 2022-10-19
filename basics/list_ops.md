# [`std::list`](https://cplusplus.com/reference/list/list/)

- List containers are implemented as doubly-linked lists;
  - Doubly linked lists can store each of the elements they contain in different and unrelated storage locations.
  - The ordering is kept internally by the association to each element of a link to the element preceding it and a link to the element following it.
- sequence containers
- constant time insert and erase operations anywhere within the sequence
- iteration in both directions.
- They are very similar to `forward_list`: The main difference being that `forward_list` objects are single-linked lists, and thus they can only be iterated forwards, in exchange for being somewhat smaller and more efficient.
- Compared to other base standard sequence containers (`array`, `vector` and `deque`), `list`s perform generally **better in inserting, extracting and moving elements in any position within the container for which an iterator has already been obtained, and therefore also in algorithms that make intensive use of these, like sorting algorithms.**
- The main drawback of `list`s and` forward_list`s compared to these other sequence containers is that they **lack direct access to the elements by their position;**
  - For example, to access the sixth element in a list, one has to iterate from a known position (like the beginning or the end) to that position, which takes linear time in the distance between these.
- They also consume some **extra memory to keep the linking information associated to each element** (which may be an important factor for large lists of small-sized elements).


# `list::splice`
- Transfers elements from x into the container, inserting them at position.
- This effectively inserts those elements into the container and **removes them from x**, altering the sizes of both containers.
- The operation does **not involve the construction or destruction of any element.**
- They are **transferred, no matter whether x is an lvalue or an rvalue, or whether the value_type supports move-construction or not.**

## signatures
- `void splice(const_iterator position, list& x)` `void splice (const_iterator position, list&& x)`
  - transfers all the elements of x into the container.
- `void splice(const_iterator position, list& x, const_iterator i)` `void splice (const_iterator position, list&& x, const_iterator i);`
  - transfers only the element pointed by i from x into the container.
- `void splice(const_iterator position, list& x, const_iterator first, const_iterator last)` `void splice(const_iterator position, list&& x, const_iterator first, const_iterator last)`
  - transfers the range [first,last) from x into the container.