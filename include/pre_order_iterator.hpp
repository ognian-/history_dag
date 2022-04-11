#pragma once

#include <utility>
#include <cstddef>
#include <iterator>
#include <stack>

#include "history_dag_node.hpp"

class PreOrderIterator {
public:
	using iterator_category = std::forward_iterator_tag;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using value_type = Node;
	using pointer = value_type*;
	using reference = value_type&;
	using const_pointer = const pointer;
	using const_reference = const reference;

	explicit PreOrderIterator(Node node);
	PreOrderIterator() = default;
	Node operator*();
	PreOrderIterator& operator++();
	PreOrderIterator operator++(int);
	bool operator==(const PreOrderIterator& other) const;
	bool operator!=(const PreOrderIterator& other) const;

private:

	static std::optional<Edge> GetFirstChild(Edge edge);
	static std::optional<Edge> GetNextSibling(Edge edge);

	std::stack<Edge> stack_;
	bool root_visited_ = false;
};
