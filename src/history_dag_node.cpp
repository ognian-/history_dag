#include "history_dag_node.hpp"
#include "history_dag.hpp"

Node::Node(HistoryDAG& dag, NodeId id) : dag_{dag}, id_{id} {}

NodeId Node::GetId() const { return id_; }

bool Node::IsRoot() const {
	return GetParents().empty();
}

bool Node::IsLeaf() {
	return GetChildren().empty();
}

NodeStorage& Node::GetStorage() const {
	return dag_.nodes_.at(id_.value);
}
