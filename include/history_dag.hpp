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
	
private:
	friend class Node;
	friend class Edge;
	
	std::vector<NodeStorage> nodes_;
	std::vector<EdgeStorage> edges_;
	NodeId root_ = {NoId};
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
