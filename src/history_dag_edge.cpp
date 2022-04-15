#include "history_dag_edge.hpp"
#include "history_dag.hpp"
#include "history_dag_node.hpp"

Edge::Edge(const HistoryDAG& dag, EdgeId id) : dag_{dag}, id_{id} {
	assert(id.value != NoId);
}

const HistoryDAG& Edge::GetDAG() const { return dag_; }

EdgeId Edge::GetId() const { return id_; }

Node Edge::GetParent() const { return {dag_, GetStorage().parent_}; }

Node Edge::GetChild() const { return {dag_, GetStorage().child_}; }

size_t Edge::GetClade() const { return GetStorage().clade_; }

std::optional<Edge> Edge::FindNextSibling() const {
	auto parent = GetParent();
    auto child = GetChild();
    auto children = parent.GetChildren();
    for (auto i = children.begin(); i != children.end(); ++i) {
        if ((*i).GetChild().GetId() == child.GetId()) {
            if (++i != children.end()) {
                return *i;
            }
            break;
        }
    }
    return std::nullopt;
}

const EdgeStorage& Edge::GetStorage() const {
	return dag_.edges_.at(id_.value);
}

bool operator==(Edge lhs, Edge rhs) {
	return std::addressof(lhs.dag_) == std::addressof(rhs.dag_) &&
		lhs.id_ == rhs.id_;
}
