#pragma once

#include <utility>
#include <cstddef>
#include <iterator>

class Node;
class HistoryDAG;

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

	PreOrderIterator(HistoryDAG& dag, Node node);
	auto operator*() const;
	PreOrderIterator& operator++();
	PreOrderIterator operator++(int) const;
	bool operator==(const PreOrderIterator& other) const;
	bool operator!=(const PreOrderIterator& other) const;

private:
	HistoryDAG& dag_;
};
