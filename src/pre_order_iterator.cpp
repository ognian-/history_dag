#include <cassert>

#include "pre_order_iterator.hpp"
#include "history_dag.hpp"

PreOrderIterator::PreOrderIterator(Node node) {
    stack_.push(*node.GetChildren().begin());
}

Node PreOrderIterator::operator*() const {
    assert(not stack_.empty());
    auto result = stack_.top();
    if (not root_visited_) return result.GetParent();
    return result.GetChild();
}

PreOrderIterator& PreOrderIterator::operator++() {
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
