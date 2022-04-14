#include "history_dag.hpp"

Edge HistoryDAG::AddEdge(EdgeId id, Node parent, Node child, size_t clade) {
	assert(id.value != NoId);
	auto& storage = GetOrInsert(edges_, id);
	storage.parent_ = parent.GetId();
	storage.child_ = child.GetId();
	storage.clade_ = clade;
	return {*this, id};
}

void HistoryDAG::Finalize() {
	root_ = {NoId};
	leafs_ = {};
	for (auto edge : GetEdges()) {
		nodes_.at(edge.GetParent().GetId().value).AddEdge(edge, true);
		nodes_.at(edge.GetChild().GetId().value).AddEdge(edge, false);
	}
	for (auto node : GetNodes()) {
		if (node.IsRoot()) {
			root_ = node.GetId();
		}
		if (node.IsLeaf()) {
			leafs_.push_back(node.GetId());
		}
	}
}

Node HistoryDAG::GetNode(NodeId id) const { return {*this, id}; }

Edge HistoryDAG::GetEdge(EdgeId id) const { return {*this, id}; }

Node HistoryDAG::GetRoot() const {
	return {*this, root_};
}
