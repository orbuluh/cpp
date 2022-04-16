#include "iteratorUseCase.h"
#include <iostream>

namespace iterator {
TreeNode::~TreeNode() {
    if (left) left.reset(nullptr);
    if (right) right.reset(nullptr);
}
TreeNode* TreeNode::setChild(bool isLeft, int val) {
    if (isLeft) {
        left.reset(new TreeNode(val, this));
        return left.get();
    }
    else {
        right.reset(new TreeNode(val, this));
        return right.get();
    }
}

NonZeroValTree::NonZeroValTree(const std::vector<int>& treeVec) {
    std::vector<TreeNode*> cache;
    if (!treeVec.empty() && treeVec.front()) {
        root.reset(new TreeNode{treeVec.front(), nullptr});
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

void IOTIterator::toBegin() {
    assert(tree);
    current = tree->root.get();
    assert(current);
    // in order traversal - begin is the left most subtree.
    while (current->left)
        current = current->left.get();
}

IOTIterator::InOrderTraversalIterator(NonZeroValTree* t, TreeNode* c) : tree(t), current(c) {
    if (!current)
        toBegin();
}

IOTIterator IOTIterator::operator++() {
    if (!current) {
        toBegin();
        return *this;
    }
    // in order traversal, you must have processed the left subtree.
    if (current->right) {
        // left subtree is done - and there is right subtree
        // so next is deepest left node of right subtree
        current = current->right.get();
        while (current->left)
            current = current->left.get();
    } else { //right tree is null
        // 3 cases where a node has no right node
        // - F is when it's some node's left child
        // - G is when it's some node's right child
        // - C is when it's root
        //         C
        //        /
        //       E
        //      / \
        //     F   G
        // F's next is E (F no right tree, but it's E's left tree, E becomes next)
        // G's next is C (G no right tree, and it's E's right tree, means that E is done, we go up)
        // C's next is end, as C is root and has no right

        auto p = current->parent;
        while (p && current == p->right.get()) {
            // current is parent->right, and current is done
            // so all tree under parent is done
            current = p;
            p = p->parent;
        }
        // when above loop break, you are either
        // 1. root node and no right subtree
        //   --> next is nullptr (end) == root->parent
        //
        // or
        //
        // 2. you are some node's left subtree, and all nodes
        //    under and include you are done, so next node is your parent.
        current = p;
    }
    return *this;
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
    std::cout << "normal dfs traverse...\n";
    dfsPrint(tree.root, 0);
    //
    std::cout << "simple iniorder iterator traverse...\n";
    IOTIterator it(&tree);
    while (it.getValue()) {
        std::cout << it.getValue() << '\n';
        ++it;
    }
}
} // namespace iterator