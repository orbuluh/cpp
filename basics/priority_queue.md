# Quick fact
- `#include <queue>`
- by default, `top()` return the **maximum** value (e.g. the default comparator is `std::less`)
- To make it a min queue, you need to explicitly use `std::greater<T>` as the third template parameter.
  - or you could have stored the negative value to reverse it basically
- ops: `top()`, `push`, `emplace()`, `pop()`
- If you want to customized comparator, it's the third argument. You have to specify container type.

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
    int idx{-1};
};

// to make a min heap, need to define the relationship as such
// that first argument is stronger than second argument:
auto comp = [](const RowProfile& r1, const RowProfile& r2) {
return (r1.score != r2.score)? (r1.score > r2.score) : (r1.idx > r2.idx); };

std::priority_queue<RowProfile, std::vector<RowProfile>, decltype(comp)> pq(comp);

// the above could be replacing with std::greater for std::pair, as pair's < operator
// would only compare second element < if first element eq

using RowProfile = std::pair<int, int>;
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