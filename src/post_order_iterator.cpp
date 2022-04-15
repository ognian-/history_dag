#include <cassert>

#include "post_order_iterator.hpp"

PostOrderIterator::PostOrderIterator(Node node) {
    stack_.push(*node.GetChildren().begin());
    PushToNextLeaf();
}

Node PostOrderIterator::operator*() {
    assert(not stack_.empty());
    if (visit_root_) return stack_.top().GetParent();
    return stack_.top().GetChild();
}

PostOrderIterator& PostOrderIterator::operator++() {
    assert(not end_sentinel_);
    assert(not stack_.empty());
    
    if (visit_root_) {
        end_sentinel_ = true;
        return *this;
    }

    Edge top = stack_.top();
    auto sibling = top.FindNextSibling();

    if (top.GetParent().IsRoot() && not sibling) {
        visit_root_ = true;
        return *this;
    }

    stack_.pop();

    if (sibling) {
        stack_.push(*sibling);
        PushToNextLeaf();
    }
    return *this;
}

PostOrderIterator PostOrderIterator::operator++(int) {
    PostOrderIterator result = *this;
    this->operator++();
    return result;
}

bool PostOrderIterator::operator==(const PostOrderIterator& other) const {
    if (other.stack_.empty() && end_sentinel_) return true;
    return stack_ == other.stack_;
}

bool PostOrderIterator::operator!=(const PostOrderIterator& other) const {
    return not (*this == other);
}

void PostOrderIterator::PushToNextLeaf() {
    while (not stack_.top().GetChild().IsLeaf()) {
        stack_.push(*stack_.top().GetChild().GetChildren().begin());
    }
}
