#include "history_dag.hpp"

void NodeStorage::ClearConnections() {
	parents_.clear();
	clades_.clear();
	leafs_below_.clear();
}

void NodeStorage::AddEdge(size_t clade, EdgeId id, bool this_node_is_parent) {
	if (this_node_is_parent) {
		GetOrInsert(clades_, clade).push_back(id);
	} else {
		parents_.push_back(id);
	}
}

void NodeStorage::RemoveEdge(Edge edge, bool this_node_is_parent) {
	if (this_node_is_parent) {
		auto& clade = clades_.at(edge.GetClade());
		auto i = std::find(clade.begin(), clade.end(), edge.GetId());
		if (i != clade.end()) clade.erase(i);
	} else {
		auto i = std::find(parents_.begin(), parents_.end(), edge.GetId());
		if (i != parents_.end()) parents_.erase(i);
	}
}
