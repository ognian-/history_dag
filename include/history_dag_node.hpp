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
	Node(const HistoryDAG& dag, NodeId id);
	const HistoryDAG& GetDAG() const;
	NodeId GetId() const;
	inline CollectionOf<char> auto GetSequence() const;
	inline CollectionOf<Edge> auto GetParents() const;
	inline CollectionOfCollections<Edge> auto GetClades() const;
	inline CollectionOf<Edge> auto GetChildren() const;
	inline CollectionOf<Node> auto GetLeafsBelow() const;
	bool IsRoot() const;
	bool IsLeaf() const;
private:
	friend bool operator==(Node, Node);
	const NodeStorage& GetStorage() const;
	const HistoryDAG& dag_;
	const NodeId id_;
};

bool operator==(Node lhs, Node rhs);

///////////////////////////////////////////////////////////////////////////////

#include "history_dag_node_storage.hpp"

CollectionOf<char> auto Node::GetSequence() const {
	return std::ranges::ref_view(GetStorage().sequence_);
}

CollectionOf<Edge> auto Node::GetParents() const {
	return GetStorage().parents_ | std::views::transform([this](EdgeId idx) {
		return Edge{dag_, idx};
	});
}

CollectionOfCollections<Edge> auto Node::GetClades() const {
	return GetStorage().clades_ | std::views::transform(
		[this](const std::vector<EdgeId>& clade) {
			return clade | std::views::transform([this](EdgeId idx) {
				return Edge{dag_, idx};
			});
		});
}

CollectionOf<Edge> auto Node::GetChildren() const {
	return GetClades() | std::views::join;
}

CollectionOf<Node> auto Node::GetLeafsBelow() const {
	return GetStorage().leafs_below_ | std::views::transform(
		[this](NodeId idx) {
			return Node{dag_, idx};
		});
}
