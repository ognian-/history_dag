#include "history_dag.hpp"

Node HistoryDAG::AddNode(NodeId id) {
	assert(id.value != NoId);
	[[maybe_unused]] auto& storage = GetOrInsert(nodes_, id);
	return {*this, id};
}

Edge HistoryDAG::AddEdge(EdgeId id, Node parent, Node child, size_t clade) {
	assert(id.value != NoId);
	auto& storage = GetOrInsert(edges_, id);
	storage.parent_ = parent.GetId();
	storage.child_ = child.GetId();
	storage.clade_ = clade;
	return {*this, id};
}

void HistoryDAG::BuildConnections() {
	root_ = {NoId};
	leafs_ = {};
	size_t edge_id = 0;
	for (auto& node : nodes_) {
		node.ClearConnections();
	}
	for (auto& edge : edges_) {
		auto& parent = nodes_.at(edge.parent_.value);
		auto& child = nodes_.at(edge.child_.value);
		parent.AddEdge(edge.clade_, {edge_id}, true);
		child.AddEdge(edge.clade_, {edge_id}, false);
		++edge_id;
	}
	for (auto node : GetNodes()) {
		if (node.IsRoot()) {
			root_ = node.GetId();
		}
		if (node.IsLeaf()) {
			leafs_.push_back(node.GetId());
		}
	}
	for (auto node : TraversePostOrder()) {
		for (auto child : node.GetChildren() |
			std::views::transform(Transform::GetChild)) {
			if (child.IsLeaf()) {
				nodes_[node.GetId().value].AddLeafsBelow(
					std::ranges::single_view{child.GetId()});
			} else {
				nodes_[node.GetId().value].AddLeafsBelow(
					child.GetLeafsBelow() |
					std::views::transform(Transform::GetId));
			}
		}
	}
}

Node HistoryDAG::GetNode(NodeId id) const { return {*this, id}; }
MutableNode HistoryDAG::GetNode(NodeId id) { return {*this, id}; }

Edge HistoryDAG::GetEdge(EdgeId id) const { return {*this, id}; }
MutableEdge HistoryDAG::GetEdge(EdgeId id) { return {*this, id}; }

Node HistoryDAG::GetRoot() const {
	return {*this, root_};
}

MutableNode HistoryDAG::GetRoot() {
	return {*this, root_};
}
