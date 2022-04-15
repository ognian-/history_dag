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
