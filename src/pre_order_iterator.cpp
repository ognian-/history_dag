#include "pre_order_iterator.hpp"

PreOrderIterator::PreOrderIterator(Node node) {
    stack_.push(*node.GetChildren().begin());
}

Node PreOrderIterator::operator*() {
    assert(not stack_.empty());
    auto result = stack_.top();
    if (!root_visited_) {
        return result.GetParent();
    }
    return result.GetChild();
}

PreOrderIterator& PreOrderIterator::operator++() {
    assert(not stack_.empty());
    auto result = stack_.top();
    if (!root_visited_) {
        root_visited_ = true;
        return *this;
    }
    if (GetFirstChild(result).has_value()) {
        stack_.push(GetFirstChild(result).value());
    } else {
        stack_.pop();
        if (GetNextSibling(result).has_value()) {
            stack_.push(GetNextSibling(result).value());
        } else {
            while (not stack_.empty() and not
                GetNextSibling(stack_.top()).has_value()) stack_.pop();
            if (not stack_.empty()) {
                auto next = GetNextSibling(stack_.top()).value();
                stack_.pop();
                stack_.push(next);
            }
        }
    }
    return *this;
}

PreOrderIterator PreOrderIterator::operator++(int) {
    PreOrderIterator result = *this;
    this->operator++();
    return result;
}

bool PreOrderIterator::operator==(const PreOrderIterator& other) const {
    return stack_ == other.stack_;
}

bool PreOrderIterator::operator!=(const PreOrderIterator& other) const {
    return not (*this == other);
}

std::optional<Edge> PreOrderIterator::GetFirstChild(Edge edge) {
    if (edge.GetChild().IsLeaf()) return std::nullopt;
    return *edge.GetChild().GetChildren().begin();
}

std::optional<Edge> PreOrderIterator::GetNextSibling(Edge edge) {
    auto parent = edge.GetParent();
    auto child = edge.GetChild();
    auto children = parent.GetChildren();
    for (auto i = children.begin(); i != children.end(); ++i) {
        if ((*i).GetChild().GetId() == child.GetId()) {
            if (++i != children.end()) {
                return *i;
            }
            break;
        }
    }
    return std::nullopt;
}
