# Quick fact
- `#include <queue>`
- by default, `top()` return the maximum value.
- ops: `top()`, `push`, `emplace()`, `pop()`
- If you want to customized comparator, it's the third argument. You have to specify container type.

```cpp
template<
    class T,
    class Container = std::vector<T>,
    class Compare = std::less<typename Container::value_type>
> class priority_queue;
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