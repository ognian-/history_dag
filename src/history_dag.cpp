#include "history_dag.hpp"

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
