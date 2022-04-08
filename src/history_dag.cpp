#include "history_dag.hpp"

Node::Node(HistoryDAG& dag, NodeId id) : dag_{dag}, id_{id} {}

NodeId Node::GetId() const { return id_; }

NodeStorage& Node::GetStorage() const {
	return dag_.nodes_.at(id_.value);
}

///////////////////////////////////////////////////////////////////////////////

Edge::Edge(HistoryDAG& dag, EdgeId id) : dag_{dag}, id_{id} {}

EdgeId Edge::GetId() const { return id_; }

Node Edge::GetParent() const { return {dag_, GetStorage().parent_}; }

Node Edge::GetChild() const { return {dag_, GetStorage().child_}; }

size_t Edge::GetClade() const { return GetStorage().clade_; }

EdgeStorage& Edge::GetStorage() const {
	return dag_.edges_.at(id_.value);
}

///////////////////////////////////////////////////////////////////////////////

void NodeStorage::AddEdge(Edge edge, bool is_parent) {
	if (is_parent) {
		GetOrInsert(clades_, edge.GetClade()).push_back(edge.GetId());
	} else {
		parents_.push_back(edge.GetId());
	}
}

///////////////////////////////////////////////////////////////////////////////

Edge HistoryDAG::AddEdge(EdgeId id, Node parent, Node child, size_t clade) {
	assert(id.value != NoId);
	auto& storage = GetOrInsert(edges_, id);
	storage.parent_ = parent.GetId();
	storage.child_ = child.GetId();
	storage.clade_ = clade;
	Edge edge{*this, id};
	nodes_.at(parent.GetId().value).AddEdge(edge, true);
	nodes_.at(child.GetId().value).AddEdge(edge, false);
	return edge;
}

Node HistoryDAG::GetNode(NodeId id) { return {*this, id}; }

Edge HistoryDAG::GetEdge(EdgeId id) { return {*this, id}; }

///////////////////////////////////////////////////////////////////////////////
