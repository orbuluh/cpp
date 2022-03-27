# `std::next_permutation`
- You need to sort first to get the full set of result from "next permutation!"

# Example: Palindrome Permutation
[Question](https://leetcode.com/problems/palindrome-permutation-ii/)
```cpp
    vector<string> generatePalindromes(string s) {
        std::unordered_map<char, int> counter;
        for (char c : s) {
            counter[c] += 1;
        }
        char oddChar = '\0';
        std::string palindromFirstHalf;
        for (auto& p : counter) {
            if (p.second % 2) {
                if (oddChar == '\0') {
                    oddChar = p.first;
                }
                else return {}; // can only have one odd char to form a palindrome
            }
            // number of count / 2 to form the first half of palindrome
            palindromFirstHalf += std::string(p.second / 2, p.first);
        }
        // need to sort to include all the permutation through std::next_permutation
        std::sort(palindromFirstHalf.begin(), palindromFirstHalf.end());
        std::vector<std::string> ans;
        do {
            std::string res = palindromFirstHalf;
            if (oddChar != '\0')
                res.push_back(oddChar);
            res += std::string(palindromFirstHalf.rbegin(), palindromFirstHalf.rend());
            ans.emplace_back(std::move(res));
        } while (std::next_permutation(palindromFirstHalf.begin(), palindromFirstHalf.end()));
        return ans;
    }
```