#pragma once

#include <vector>
#include <set>

#include "history_dag_common.hpp"

class NodeStorage {
	template <typename> friend class NodeView;
	friend class HistoryDAG;
	
	void ClearConnections();
	void AddEdge(size_t clade, EdgeId id, bool this_node_is_parent);
	void RemoveEdge(Edge edge, bool this_node_is_parent);
	void AddLeafsBelow(CollectionOf<NodeId> auto&& leafs);


	std::vector<EdgeId> parents_;
	std::vector<std::vector<EdgeId>> clades_;
	std::set<NodeId> leafs_below_;
};

///////////////////////////////////////////////////////////////////////////////

void NodeStorage::AddLeafsBelow(CollectionOf<NodeId> auto&& leafs) {
	leafs_below_.insert(std::begin(leafs), std::end(leafs));
}
