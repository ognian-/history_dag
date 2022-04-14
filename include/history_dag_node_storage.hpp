#pragma once

#include <vector>

#include "history_dag_edge.hpp"

class NodeStorage {
	friend class Node;
	friend class HistoryDAG;
	
	void AddEdge(Edge edge, bool is_parent);

	std::vector<char> sequence_;
	std::vector<EdgeId> parents_;
	std::vector<std::vector<EdgeId>> clades_;
	std::vector<NodeId> leafs_below_;
};
