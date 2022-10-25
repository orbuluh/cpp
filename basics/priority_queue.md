# Quick fact
- Working with a `priority_queue` is similar to managing a heap in some random access container, **with the benefit of not being able to accidentally invalidate the heap.**
  - e.g. When you use `make_heap` you still have access to all elements. When you use `priority_queue`, you have only a few operations giving very limited access to elements.
- `#include <queue>`
- by default, `top()` return the **maximum** value (e.g. the default comparator is `std::less<T>`)
  - A user-provided Compare can be supplied to change the ordering, e.g. using `std::greater<T>` would cause the smallest element to appear as the `top()` (e.g. becomes min priority queue).
  - or you could have stored the negative value to reverse it basically
- ops: `top()`, `push`, `emplace()`, `pop()`
- If you want to customized comparator, it's the third argument. You have to specify container type.

# Complexity
- `template<class InputIt> priority_queue( InputIt first, InputIt last, ...)`: O(M) comparisons, where M is `std::distance(first, last)` - basically a heapify
- `top()` is O(1) but `pop()` is O(log n) comparisons
- `push()`, `emplace()` is basically O(log n) comparisons

# Applicability
- `{1, 2, 3, 4, 5, 6}`
- 2-th (k = 2) largest is 5, you can think it in two way:
  - maintain a minPQ, with size k = 2. Keep pushing element and pop top (current min) out. Then at the end, the top element is the 2 (k-th) largest.
  - maintain a maxPQ, pushing all element then pop k - 1 = 1 element. Then the top is the 2 (k-th) largest
- [Top K Frequent Elements](https://leetcode.com/problems/top-k-frequent-elements/)
- [Kth Largest Element in an Array](https://leetcode.com/problems/kth-largest-element-in-an-array/)
  - There is actually a quicker way -> partition (in quick sort) to find the k-th largest ... [note to be added]

# Customized comparator
```cpp
template<
    class T,
    class Container = std::vector<T>,
    class Compare = std::less<typename Container::value_type>
> class priority_queue;
```
# `Compare`
- `Compare` is A type providing a strict weak ordering.
- Note that the `Compare` parameter is defined such that *it returns true if its first argument comes before its second argument in a weak ordering.*
- But because the *priority queue outputs largest elements first, the elements that "come before" are actually output last.*
- That is, the front of the queue contains the "last" element according to the weak ordering imposed by Compare.

# Example, customized comparator to create min-heap
```cpp
struct RowProfile {
    int score{0};
    std::string_view name;
};

// r1 comes before r2 iff r1.score is larger than r2.score or if r1.name is
// larger than r2.name. Because priority queue outputs later element first, it
// means RowProfile with lower score or lexicographically smaller name will be
// output first!
auto comp = [](const RowProfile& r1, const RowProfile& r2) {
    return (r1.score != r2.score)? (r1.score > r2.score) : (r1.name > r2.name);
};

std::priority_queue<RowProfile, std::vector<RowProfile>, decltype(comp)> pq(comp);

// the above could be replacing with std::greater for std::pair, as pair's < operator
// would only compare second element < if first element eq

using RowProfile = std::pair<int, std::string_view>;
std::priority_queue<RowProfile, std::vector<RowProfile>, std::greater<RowProfile>> pq();

// on the contrary, by default, this will be a max-heap
// e.g. comparator is implicitly std::less<RowProfile> basically
std::priority_queue<RowProfile> pq();
```

# Example, customized comparator
[Max Value of Equation](https://leetcode.com/problems/max-value-of-equation/)
```cpp
#include <queue>
#include <limits>
class Solution {
    struct Point {
        Point(const std::vector<int>& vec) : x(vec[0]), y(vec[1]) {}
        int x;
        int y;
    };
public:
    // when j > i, yi + yj + |xi - xj| = (yi - xi) + (xj + yj)
    // for any point (xj, yj), just need to find the maximum (yi -xi) such that j - i <= k
    int findMaxValueOfEquation(vector<vector<int>>& points, int k) {
        auto comp = [](const Point& a, const Point& b) -> bool {
            // max pq that maintains the max of y - x
            return (a.y - a.x)  < (b.y - b.x);
        };
        std::priority_queue<Point, std::vector<Point>, decltype(comp)> pq(comp);
        int curMax = std::numeric_limits<int>::min();
        for (const auto& ptVec : points) {
            Point p{ptVec};
            while (!pq.empty() && p.x - pq.top().x > k) {
                // maintain the pq so that the point with maximum (y - x) value contains x
                // that is within k distance from the new point
                pq.pop();
            }
            if (!pq.empty()) {
                // if pq not empty, and because we pop invalid max node already,
                // the top node must be qualified to form the answer.
                curMax = max(curMax, p.x + p.y + pq.top().y - pq.top().x);
            }
            pq.emplace(std::move(p));
        }
        return curMax;
    }
};
```