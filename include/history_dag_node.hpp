#pragma once

#include "history_dag_common.hpp"

class HistoryDAG;
class NodeStorage;
class Edge;

class Node {
public:
	Node(HistoryDAG& dag, NodeId id);
	NodeId GetId() const;
	inline auto GetSequence() const;
	inline auto GetParents() const;
	inline auto GetClades() const;
	inline auto GetChildren() const;
	inline auto GetLeafsBelow() const;
private:
	NodeStorage& GetStorage() const;
	HistoryDAG& dag_;
	NodeId id_;
};

///////////////////////////////////////////////////////////////////////////////

#include "history_dag_node_storage.hpp"
#include "collection.hpp"
#include "flat_collection.hpp"

auto Node::GetSequence() const {
	return Collection{GetStorage().sequence_, [](char x, size_t) {
		return x;
	}};
}

auto Node::GetParents() const {
	return Collection{GetStorage().parents_, [this](EdgeId idx, size_t) {
		return Edge{dag_, idx};
	}};
}

auto Node::GetClades() const {
	return Collection{GetStorage().clades_,
		[this](std::vector<EdgeId>& clade, size_t) {
		return Collection{clade, [this](EdgeId idx, size_t) {
			return Edge{dag_, idx};
		}};
	}};
}

auto Node::GetChildren() const {
	return FlatCollection{GetClades()};
}

auto Node::GetLeafsBelow() const {
	return Collection{GetStorage().leafs_below_, [this](NodeId idx, size_t) {
		return Node{dag_, idx};
	}};
}
