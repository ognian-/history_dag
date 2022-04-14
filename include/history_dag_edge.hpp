/*
  Edge is a lightweight view object into the internal edge storage of a
  HistoryDAG. It is meant to be cheaply passed by value, and behaves as a
  reference into raw storage that conveniently enriches it's API.
*/
#pragma once

#include "history_dag_common.hpp"

class HistoryDAG;
class EdgeStorage;
class Node;

class Edge {
public:
	Edge(const HistoryDAG& dag, EdgeId id);
	const HistoryDAG& GetDAG() const;
	EdgeId GetId() const;
	Node GetParent() const;
	Node GetChild() const;
	size_t GetClade() const;
private:
	friend bool operator==(Edge, Edge);
	const EdgeStorage& GetStorage() const;
	const HistoryDAG& dag_;
	const EdgeId id_;
};

bool operator==(Edge lhs, Edge rhs);
