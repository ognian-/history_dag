/*
  Node is a lightweight view object into the internal node storage of a
  HistoryDAG. It is meant to be cheaply passed by value, and behaves as a
  reference into raw storage that conveniently enriches it's API.
*/
#pragma once

#include "history_dag_common.hpp"

class HistoryDAG;
class NodeStorage;
class Edge;

class Node {
public:
	Node(HistoryDAG& dag, NodeId id);
	NodeId GetId() const;
	inline auto GetSequence();
	inline auto GetParents() const;
	inline auto GetClades();
	inline auto GetChildren();
	inline auto GetLeafsBelow() const;
	bool IsRoot() const;
	bool IsLeaf();
private:
	friend bool operator==(Node, Node);
	NodeStorage& GetStorage() const;
	HistoryDAG& dag_;
	NodeId id_;
};

bool operator==(Node lhs, Node rhs);

///////////////////////////////////////////////////////////////////////////////

#include "history_dag_node_storage.hpp"
#include "collection.hpp"
#include "flat_collection.hpp"

auto Node::GetSequence() {
	return Collection{GetStorage().sequence_, [](char x, size_t) {
		return x;
	}};
}

auto Node::GetParents() const {
	return Collection{GetStorage().parents_, [this](EdgeId idx, size_t) {
		return Edge{dag_, idx};
	}};
}

auto Node::GetClades() {
	return Collection{GetStorage().clades_,
		[this](std::vector<EdgeId>& clade, size_t) {
		return Collection{clade, [this](EdgeId idx, size_t) {
			return Edge{dag_, idx};
		}};
	}};
}

auto Node::GetChildren() {
	return FlatCollection{GetClades()};
}

auto Node::GetLeafsBelow() const {
	return Collection{GetStorage().leafs_below_, [this](NodeId idx, size_t) {
		return Node{dag_, idx};
	}};
}
