#include "iteratorUseCase.h"
#include <iostream>

namespace iterator {
TreeNode::~TreeNode() {
    if (left) left.reset(nullptr);
    if (right) right.reset(nullptr);
}
TreeNode* TreeNode::setChild(bool isLeft, int val) {
    if (isLeft) {
        left.reset(new TreeNode(val));
        return left.get();
    }
    else {
        right.reset(new TreeNode(val));
        return right.get();
    }
}

NonZeroValTree::NonZeroValTree(const std::vector<int>& treeVec) {
    std::vector<TreeNode*> cache;
    if (!treeVec.empty() && treeVec.front()) {
        root.reset(new TreeNode{treeVec.front()});
        cache.push_back(root.get());
    }
    for (int i = 1; i < treeVec.size(); ++i) {
        if (i % 2) {
            assert(cache[i/2]);
            if (treeVec[i])
                cache.push_back(cache[i / 2]->setChild(true, treeVec[i]));
            else
                cache.push_back(nullptr);
        } else {
            assert(cache[i / 2 - 1]);
            if (treeVec[i])
                cache.push_back(cache[i / 2 - 1]->setChild(false, treeVec[i]));
            else
                cache.push_back(nullptr);
        }
    }
}

void dfsPrint(const std::unique_ptr<TreeNode>& node, int depth) {
    if (!node) return;
    const auto prefix = std::string(depth * 2, ' ');
    std::cout << prefix << node->val << '\n';
    dfsPrint(node->left, depth + 1);
    dfsPrint(node->right, depth + 1);
}

void demo() {
    std::vector<int> treeVec = { 1, 2, 3, 4, 5, 6, 7, 8 };
    NonZeroValTree tree(treeVec);
    dfsPrint(tree.root, 0);
}
} // namespace iterator