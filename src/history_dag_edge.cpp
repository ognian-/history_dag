#include "history_dag_edge.hpp"
#include "history_dag.hpp"
#include "history_dag_node.hpp"

Edge::Edge(HistoryDAG& dag, EdgeId id) : dag_{dag}, id_{id} {}

EdgeId Edge::GetId() const { return id_; }

Node Edge::GetParent() const { return {dag_, GetStorage().parent_}; }

Node Edge::GetChild() const { return {dag_, GetStorage().child_}; }

size_t Edge::GetClade() const { return GetStorage().clade_; }

EdgeStorage& Edge::GetStorage() const {
	return dag_.edges_.at(id_.value);
}
