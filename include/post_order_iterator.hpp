#pragma once

#include <utility>
#include <cstddef>
#include <iterator>
#include <stack>
#include <optional>

#include "history_dag_common.hpp"

template <typename NodeType>
class PostOrderIterator {
public:
	using EdgeType = decltype(*std::declval<NodeType>().GetChildren().begin());
	
	using iterator_category = std::forward_iterator_tag;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using value_type = NodeType;
	using pointer = value_type*;
	using reference = value_type&;
	using const_pointer = const pointer;
	using const_reference = const value_type&;

	explicit PostOrderIterator(NodeType node);
	PostOrderIterator() = default;
	NodeType operator*() const;
	EdgeType GetEdge() const;
	PostOrderIterator& operator++();
	PostOrderIterator operator++(int);
	bool operator==(const PostOrderIterator& other) const;
	bool operator!=(const PostOrderIterator& other) const;

private:
    
	void PushToNextLeaf();
	auto GetCurrent() const;

	std::stack<EdgeType> stack_;
    bool visit_root_ = false;
    bool end_sentinel_ = false;
};
