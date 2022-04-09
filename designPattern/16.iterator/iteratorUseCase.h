#pragma once

#include <vector>
#include <memory>
namespace iterator {

struct TreeNode {
    TreeNode(int v, TreeNode* p) : val(v), parent(p) {}
    ~TreeNode();
    TreeNode* setChild(bool isLeft, int val);
    int val;
    TreeNode* parent;
    std::unique_ptr<TreeNode> left;
    std::unique_ptr<TreeNode> right;
};

class NonZeroValTree {
    //treeVec is an vec for a complete tree
public:
    NonZeroValTree(const std::vector<int>& treeVec);
    std::unique_ptr<TreeNode> root;
public:
    class InOrderTraversalIterator {
    public:
        InOrderTraversalIterator(NonZeroValTree* t, TreeNode* c = nullptr);
        InOrderTraversalIterator operator++();
        int getValue() {
            if (current)
                return current->val;
            return 0;
        }
    private:
        void toBegin();
        const NonZeroValTree* tree;
        TreeNode* current;
    };
};

using IOTIterator = NonZeroValTree::InOrderTraversalIterator;

void demo();
} // namespace iterator