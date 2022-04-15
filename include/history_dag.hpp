/*
  HistoryDAG is the main structure that owns node and edge data, and provides
  various queries.

  Populating with data should be performed by first adding all nodes by the
  AddNode() function, then adding all the edges with AddEdge() and finally
  calling Finalize().

  NodeId and EdgeId are strongly typed wrappers around size_t, and data is
  stored internally by the order of its IDs.

  GetNodes() and GetEdges() returns a view into the corresponding elements,
  ordered by id.

  TraversePreOrder() returns a view into Nodes in pre-order traversal order.

*/
#pragma once

#include <type_traits>
#include <limits>
#include <iterator>
#include <algorithm>
#include <cassert>
#include <vector>
#include <ranges>

#include "history_dag_node_storage.hpp"
#include "history_dag_edge_storage.hpp"
#include "pre_order_iterator.hpp"
#include "post_order_iterator.hpp"

class HistoryDAG {
public:
	
	template <typename Sequence>
	Node AddNode(NodeId id, Sequence&& sequence);
	
	Edge AddEdge(EdgeId id, Node parent, Node child, size_t clade);

	void Finalize();
	
	inline CollectionOf<Node> auto GetNodes() const;
	inline CollectionOf<Edge> auto GetEdges() const;
	
	Node GetNode(NodeId id) const;
	Edge GetEdge(EdgeId id) const;

	Node GetRoot() const;

	inline CollectionOf<Node> auto GetLeafs() const;

	inline CollectionOf<Node> auto TraversePreOrder() const;
	inline CollectionOf<Node> auto TraversePostOrder() const;
	
private:
	friend class Node;
	friend class Edge;
	
	std::vector<NodeStorage> nodes_;
	std::vector<EdgeStorage> edges_;
	NodeId root_ = {NoId};
	std::vector<NodeId> leafs_;
};

///////////////////////////////////////////////////////////////////////////////

#include "history_dag_node.hpp"

template <typename Sequence>
Node HistoryDAG::AddNode(NodeId id, Sequence&& sequence) {
	assert(id.value != NoId);
	auto& storage = GetOrInsert(nodes_, id);
	storage.sequence_.clear();
	std::copy(sequence.begin(), sequence.end(),
		std::back_inserter(storage.sequence_));
	return {*this, id};
}

CollectionOf<Node> auto HistoryDAG::GetNodes() const {
	return nodes_ | std::views::transform(
		[this, idx = size_t{}](const NodeStorage&) mutable {
			return Node{*this, {idx++}};
		});
}

CollectionOf<Edge> auto HistoryDAG::GetEdges() const {
	return edges_ | std::views::transform(
		[this, idx = size_t{}](const EdgeStorage&) mutable {
			return Edge{*this, {idx++}};
		});
}

CollectionOf<Node> auto HistoryDAG::GetLeafs() const {
	return leafs_ | std::views::transform([this](NodeId node_id) {
			return Node{*this, node_id};
		});
}

CollectionOf<Node> auto HistoryDAG::TraversePreOrder() const {
	return std::ranges::subrange(PreOrderIterator{GetRoot()},
		PreOrderIterator{});
}

CollectionOf<Node> auto HistoryDAG::TraversePostOrder() const {
	return std::ranges::subrange(PostOrderIterator{GetRoot()},
		PostOrderIterator{});
}
