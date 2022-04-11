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

#include "flat_collection.hpp"
#include "collection.hpp"

#include <type_traits>
#include <limits>
#include <iterator>
#include <algorithm>
#include <cassert>
#include <vector>

#include "history_dag_node_storage.hpp"
#include "history_dag_edge_storage.hpp"
#include "pre_order_iterator.hpp"

class HistoryDAG {
public:
	
	template <typename Sequence>
	Node AddNode(NodeId id, Sequence&& sequence);
	
	Edge AddEdge(EdgeId id, Node parent, Node child, size_t clade);

	void Finalize();
	
	inline auto GetNodes();
	inline auto GetEdges();
	
	Node GetNode(NodeId id);
	Edge GetEdge(EdgeId id);

	Node GetRoot();

	inline auto GetLeafs();

	inline auto TraversePreOrder();
	
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

auto HistoryDAG::GetNodes() {
	return Collection{nodes_, [this](NodeStorage&, size_t idx) {
		return Node{*this, {idx}};
	}};
}

auto HistoryDAG::GetEdges() {
	return Collection{edges_, [this](EdgeStorage&, size_t idx) {
		return Edge{*this, {idx}};
	}};
}

inline auto HistoryDAG::GetLeafs() {
	return Collection{leafs_, [this](NodeId node_id, size_t) {
		return Node{*this, node_id};
	}};
}

auto HistoryDAG::TraversePreOrder() {
	return Range{PreOrderIterator{GetRoot()}, PreOrderIterator{}};
}
