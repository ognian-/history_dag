#include "history_dag_node_storage.hpp"

void NodeStorage::AddEdge(Edge edge, bool is_parent) {
	if (is_parent) {
		GetOrInsert(clades_, edge.GetClade()).push_back(edge.GetId());
	} else {
		parents_.push_back(edge.GetId());
	}
}
