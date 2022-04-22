#include "history_dag.hpp"

MutableNode HistoryDAG::AddNode(NodeId id) {
	assert(id.value != NoId);
	[[maybe_unused]] auto& storage = GetOrInsert(nodes_, id);
	[[maybe_unused]] auto& leafs_below = GetOrInsert(nodes_leafs_below_, id);
	[[maybe_unused]] auto& label = GetOrInsert(nodes_labels_, id);
	return {*this, id};
}

MutableEdge HistoryDAG::AddEdge(EdgeId id, Node parent, Node child,
	size_t clade) {
	return AddEdge(id, parent.GetId(), child.GetId(), clade);
}

MutableEdge HistoryDAG::AddEdge(EdgeId id, NodeId parent, NodeId child,
	size_t clade) {
	assert(id.value != NoId);
	auto& storage = GetOrInsert(edges_, id);
	storage.parent_ = parent;
	storage.child_ = child;
	storage.clade_ = clade;
	[[maybe_unused]] auto& mutations = GetOrInsert(edges_mutations_, id);
	return {*this, id};
}

void HistoryDAG::BuildConnections() {
	root_ = {NoId};
	leafs_ = {};
	nodes_leafs_below_.clear();
	nodes_leafs_below_.resize(nodes_.size());

	for (auto& node : nodes_) {
		node.ClearConnections();
	}
	size_t edge_id = 0;
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
				nodes_leafs_below_[node.GetId().value].insert(child.GetId());
			} else {
				auto leafs = child.GetLeafsBelow() |
					std::views::transform(Transform::GetId);
				nodes_leafs_below_[node.GetId().value].insert(
					std::begin(leafs), std::end(leafs));
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

size_t HistoryDAG::HashOfNode(Node node) {
	auto leaf_mutations = node.GetChildren() | std::views::transform(
			[](Edge child) {
				return child.GetMutations();
			});

	return HashCombine(HashOf((*node.GetParents().begin()).GetMutations()),
		HashOf(leaf_mutations | std::views::join));
}

bool HistoryDAG::Equal(Node lhs, Node rhs) {
	if (not std::ranges::equal((*lhs.GetParents().begin()).GetMutations(),
		(*rhs.GetParents().begin()).GetMutations())) return false;
	
	auto lhs_leaf_mutations = lhs.GetChildren() | std::views::transform(
			[](Edge child) {
				return child.GetMutations();
			});

	auto rhs_leaf_mutations = rhs.GetChildren() | std::views::transform(
			[](Edge child) {
				return child.GetMutations();
			});

	if (not std::ranges::equal(lhs_leaf_mutations |
		std::views::join, rhs_leaf_mutations | std::views::join)) return false;

	return true;
}
