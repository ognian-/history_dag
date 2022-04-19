#include <cassert>

template <typename NodeType>
PreOrderIterator<NodeType>::PreOrderIterator(NodeType node) {
    stack_.push(*node.GetChildren().begin());
}

template <typename NodeType>
NodeType PreOrderIterator<NodeType>::operator*() const {
    assert(not stack_.empty());
    auto result = stack_.top();
    if (not root_visited_) return result.GetParent();
    return result.GetChild();
}

template <typename NodeType>
PreOrderIterator<NodeType>& PreOrderIterator<NodeType>::operator++() {
    assert(not stack_.empty());
    auto top = stack_.top();
    if (not root_visited_) {
        root_visited_ = true;
        return *this;
    }
    if (GetFirstChild(top).has_value()) {
        stack_.push(GetFirstChild(top).value());
    } else {
        stack_.pop();
        if (top.FindNextSibling().has_value()) {
            stack_.push(*top.FindNextSibling());
        } else {
            while (not stack_.empty() and not
                stack_.top().FindNextSibling().has_value()) stack_.pop();
            if (not stack_.empty()) {
                auto next = *stack_.top().FindNextSibling();
                stack_.pop();
                stack_.push(next);
            }
        }
    }
    return *this;
}

template <typename NodeType>
PreOrderIterator<NodeType> PreOrderIterator<NodeType>::operator++(int) {
    PreOrderIterator result = *this;
    this->operator++();
    return result;
}

template <typename NodeType>
bool PreOrderIterator<NodeType>::operator==(
    const PreOrderIterator& other) const {
    return stack_ == other.stack_;
}

template <typename NodeType>
bool PreOrderIterator<NodeType>::operator!=(
    const PreOrderIterator& other) const {
    return not (*this == other);
}

template <typename NodeType>
std::optional<typename PreOrderIterator<NodeType>::EdgeType>
PreOrderIterator<NodeType>::GetFirstChild(EdgeType edge) {
    if (edge.GetChild().IsLeaf()) return std::nullopt;
    return *edge.GetChild().GetChildren().begin();
}
