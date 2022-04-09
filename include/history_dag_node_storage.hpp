#pragma once

#include <vector>

#include "history_dag_edge.hpp"

class NodeStorage {
public:
	
	void AddEdge(Edge edge, bool is_parent);

	std::vector<char> sequence_;
	std::vector<EdgeId> parents_;
	std::vector<std::vector<EdgeId>> clades_;
	std::vector<NodeId> leafs_below_;
};
