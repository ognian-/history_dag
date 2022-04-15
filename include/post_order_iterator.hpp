#pragma once

#include <utility>
#include <cstddef>
#include <iterator>
#include <stack>
#include <optional>

#include "history_dag_node.hpp"

class PostOrderIterator {
public:
	using iterator_category = std::forward_iterator_tag;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using value_type = Node;
	using pointer = value_type*;
	using reference = value_type&;
	using const_pointer = const pointer;
	using const_reference = const reference;

	explicit PostOrderIterator(Node node);
	PostOrderIterator() = default;
	Node operator*();
	PostOrderIterator& operator++();
	PostOrderIterator operator++(int);
	bool operator==(const PostOrderIterator& other) const;
	bool operator!=(const PostOrderIterator& other) const;

private:
    void PushToNextLeaf();

	std::stack<Edge> stack_;
    bool visit_root_ = false;
    bool end_sentinel_ = false;
};
