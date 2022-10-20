[code](unordered_map_ops.h)

# [Customized hash for user-defined key-type](https://stackoverflow.com/a/17017281/4924135)
You need to define two things:
> **A hash function**
- this must be a class that overrides `operator()` and calculates the hash value given an object of the key-type.
- One particularly straight-forward way of doing this is to specialize the `std::hash` template for your key-type.
> **A comparison function for equality**
- this is required because the hash cannot rely on the fact that the hash function will always provide a unique hash value for every distinct key (i.e., it needs to be able to deal with collisions), so it needs a way to compare two given keys for an exact match.
- You can implement this either as a class that overrides operator(), or as a specialization of `std::equal`, or – easiest of all – by overloading `operator==()` for your key type (as you did already).

The difficulty with the hash function is that if your key type consists of several members, you will usually have the hash function calculate hash values for the individual members, and then somehow combine them into one hash value for the entire object.
- For good performance (i.e., few collisions) you should think carefully about how to combine the individual hash values to ensure you avoid getting the same output for different objects too often.
- A fairly good starting point for a hash function is one that uses **bit shifting** and **bitwise XOR** to combine the individual hash values.


# About `insert` - [from response](https://stackoverflow.com/a/4286924/4924135)

## `operator[]` and `insert` member functions are not functionally equivalent :
- The `operator[]` will search for the key, insert a **default constructed value** if not found, and return a reference to which you assign a value.
- This method also makes it impossible to determine if an insertion has indeed taken place or if you have only overwritten the value for an previously inserted key
- Obviously, this can be inefficient if the mapped_type can benefit from being directly initialized instead of default constructed and assigned.

- The `insert` member function will **have no effect if the key is already present in the map** and, although it is often forgotten, returns an `std::pair<iterator, bool>` which can be of interest (most notably to determine if insertion has actually been done).

- I would avoid using `operator[]` when the objective is to insert, unless there is no additional cost in default-constructing and assigning the mapped_type, and that I don't care about determining if a new key has effectively inserted.
- When using `insert`, constructing a `value_type` is probably the way to go.

# Use `std::unordered_map` with a `value_type` without default constructor?
```cpp
        /*
        struct Edge {
            int srcIdx = 0;
            int tgtIdx = 0;
            int weight = 0;
        };
        */
        auto comp = [](Edge* a, Edge* b) {
            return (*a)[2] > (*b)[2];
        };
        using min_pq_t = std::priority_queueEdge*,
                            std::vector<Edge*>,
                            decltype(comp)>;
        std::unordered_map<int, min_pq_t> graph;
        for (auto& edge : edges) { // some input std::vector<Edge> edges
            auto insertPair = graph.insert({edge.srcIdx, min_pq_t(comp)});
            auto& pq = insertPair.first->second;
            pq.push(&edge);
        }
```
