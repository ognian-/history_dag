#pragma once

#include "history_dag_common.hpp"

class HistoryDAG;
class EdgeStorage;
class Node;

class Edge {
public:
	Edge(HistoryDAG& dag, EdgeId id);
	EdgeId GetId() const;
	Node GetParent() const;
	Node GetChild() const;
	size_t GetClade() const;
private:
	EdgeStorage& GetStorage() const;
	HistoryDAG& dag_;
	EdgeId id_;
};
