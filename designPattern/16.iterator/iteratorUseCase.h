#pragma once

#include <vector>
#include <memory>
namespace iterator {

struct TreeNode {
    TreeNode(int v) : val(v) {}
    ~TreeNode();
    TreeNode* setChild(bool isLeft, int val);
    int val;
    std::unique_ptr<TreeNode> left;
    std::unique_ptr<TreeNode> right;
};

struct NonZeroValTree {
    //treeVec is an vec for a complete tree
    NonZeroValTree(const std::vector<int>& treeVec);
    std::unique_ptr<TreeNode> root;
};




void demo();
} // namespace iterator