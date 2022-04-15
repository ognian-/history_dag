#pragma once

#include <vector>

#include "history_dag_common.hpp"

class NodeStorage {
	template <typename> friend class NodeView;
	friend class HistoryDAG;
	
	void ClearConnections();
	void AddEdge(size_t clade, EdgeId id, bool this_node_is_parent);

	std::vector<char> sequence_;
	std::vector<EdgeId> parents_;
	std::vector<std::vector<EdgeId>> clades_;
	std::vector<NodeId> leafs_below_;
};
