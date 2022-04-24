# Quick fact
* `std::find` doesn't take customized comparator - `std::find_if` or `std::find_if_not` does.
* `find_if` predicate example - input type is the element type of the container.
```cpp
class NaiveHashMap {
    static constexpr int bucketCnt_ = 23;
    using KeyVal = std::pair<int, int>;
    using Bucket = std::list<KeyVal>;
    std::vector<Bucket> buckets_;

    int hash(int key) { return key % bucketCnt_; }
public:
    MyHashMap() {
        buckets_.resize(bucketCnt_);
    }

    int get(int key) {
        auto& bucket = buckets_[hash(key)];
        auto it = std::find_if(bucket.begin(), bucket.end(),
                  [&](const auto& p) { return p.first == key; });
        if (it == bucket.end() || it->first != key)
            return -1;
        else
            return it->second;
    }
    //...
};
```

# Related topic
- [Heterogenous lookup](heterogenous_lookup.md)