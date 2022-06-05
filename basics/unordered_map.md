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
