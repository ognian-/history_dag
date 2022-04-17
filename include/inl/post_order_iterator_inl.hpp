#include <cassert>

template <typename NodeType>
PostOrderIterator<NodeType>::PostOrderIterator(NodeType node) {
    stack_.push(*node.GetChildren().begin());
    PushToNextLeaf();
}

template <typename NodeType>
template <typename, typename>
MutableNode PostOrderIterator<NodeType>::operator*() const {
    return GetCurrent();
}

template <typename NodeType>
Node PostOrderIterator<NodeType>::operator*() const {
    return GetCurrent();
}

template <typename NodeType>
PostOrderIterator<NodeType>& PostOrderIterator<NodeType>::operator++() {
    assert(not end_sentinel_);
    assert(not stack_.empty());
    
    if (visit_root_) {
        end_sentinel_ = true;
        return *this;
    }

    EdgeType top = stack_.top();
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

template <typename NodeType>
PostOrderIterator<NodeType> PostOrderIterator<NodeType>::operator++(int) {
    PostOrderIterator result = *this;
    this->operator++();
    return result;
}

template <typename NodeType>
bool PostOrderIterator<NodeType>::operator==(
    const PostOrderIterator& other) const {
    if (other.stack_.empty() && end_sentinel_) return true;
    return stack_ == other.stack_;
}

template <typename NodeType>
bool PostOrderIterator<NodeType>::operator!=(
    const PostOrderIterator& other) const {
    return not (*this == other);
}

template <typename NodeType>
void PostOrderIterator<NodeType>::PushToNextLeaf() {
    while (not stack_.top().GetChild().IsLeaf()) {
        stack_.push(*stack_.top().GetChild().GetChildren().begin());
    }
}

template <typename NodeType>
auto PostOrderIterator<NodeType>::GetCurrent() const {
    assert(not stack_.empty());
    if (visit_root_) return stack_.top().GetParent();
    return stack_.top().GetChild();
}