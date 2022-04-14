#include "history_dag_node.hpp"
#include "history_dag.hpp"

Node::Node(const HistoryDAG& dag, NodeId id) : dag_{dag}, id_{id} {
	assert(id.value != NoId);
}

const HistoryDAG& Node::GetDAG() const { return dag_; }

NodeId Node::GetId() const { return id_; }

bool Node::IsRoot() const {
	return GetParents().empty();
}

bool Node::IsLeaf() const {
	return GetChildren().begin() == GetChildren().end();
}

const NodeStorage& Node::GetStorage() const {
	return dag_.nodes_.at(id_.value);
}

bool operator==(Node lhs, Node rhs) {
	return std::addressof(lhs.dag_) == std::addressof(rhs.dag_) &&
		lhs.id_ == rhs.id_;
}
