#pragma once

#include <utility>
#include <cstddef>
#include <iterator>
#include <stack>

#include "history_dag_common.hpp"

template <typename NodeType>
class PreOrderIterator {
public:
	using iterator_category = std::forward_iterator_tag;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using value_type = NodeType;
	using pointer = value_type*;
	using reference = value_type&;
	using const_pointer = const pointer;
	using const_reference = const value_type&;

	explicit PreOrderIterator(NodeType node);
	PreOrderIterator() = default;
	NodeType operator*() const;
	PreOrderIterator& operator++();
	PreOrderIterator operator++(int);
	bool operator==(const PreOrderIterator& other) const;
	bool operator!=(const PreOrderIterator& other) const;

private:
	using EdgeType = decltype(*std::declval<NodeType>().GetChildren().begin());

	static std::optional<EdgeType> GetFirstChild(EdgeType edge);

	std::stack<EdgeType> stack_;
	bool root_visited_ = false;
};
