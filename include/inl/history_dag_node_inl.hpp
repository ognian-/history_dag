#include <set>
#include "history_dag_node_storage.hpp"

template <typename T>
NodeView<T>::NodeView(T dag, NodeId id) : dag_{dag}, id_{id} {
	static_assert(std::is_same_v<T, HistoryDAG&> or
        std::is_same_v<T, const HistoryDAG&>);
	assert(id.value != NoId);
}

template <typename T>
NodeView<T>::operator Node() const {
	return {dag_, id_};
}

template <typename T>
const HistoryDAG& NodeView<T>::GetDAG() const { return dag_; }

template <typename T>
NodeId NodeView<T>::GetId() const { return id_; }

template <typename T>
CollectionOf<char> auto NodeView<T>::GetSequence() const {
	return std::ranges::ref_view(GetStorage().sequence_);
}

template <typename T>
CollectionOf<Edge> auto NodeView<T>::GetParents() const {
	return GetStorage().parents_ | std::views::transform([this](EdgeId idx) {
		return Edge{dag_, idx};
	});
}

template <typename T>
CollectionOfCollections<Edge> auto NodeView<T>::GetClades() const {
	return GetStorage().clades_ | std::views::transform(
		[this](const std::vector<EdgeId>& clade) {
			return clade | std::views::transform([this](EdgeId idx) {
				return Edge{dag_, idx};
			});
		});
}

template <typename T>
CollectionOf<Edge> auto NodeView<T>::GetChildren() const {
	return GetClades() | std::views::join;
}

template <typename T>
CollectionOf<Node> auto NodeView<T>::GetLeafsBelow() const {
	return GetStorage().leafs_below_ | std::views::transform(
		[this](NodeId idx) {
			return Node{dag_, idx};
		});
}

template <typename T>
bool NodeView<T>::IsRoot() const {
	return GetParents().empty();
}

template <typename T>
bool NodeView<T>::IsLeaf() const {
	return GetChildren().begin() == GetChildren().end();
}

template <typename T>
void NodeView<T>::AddParentEdge(Edge edge) {
	GetStorage().AddEdge(edge.GetClade(), edge.GetId(), false);
}

template <typename T>
void NodeView<T>::AddChildEdge(Edge edge) {
	GetStorage().AddEdge(edge.GetClade(), edge.GetId(), true);
}

template <typename T>
void NodeView<T>::RemoveParentEdge(Edge edge) {
	GetStorage().RemoveEdge(edge, false);
}

template <typename T>
auto NodeView<T>::BuildMutsRelReference() const {
	std::set<Mutation> muts;
    NodeId current_node = id_;
    while (not dag_.GetNode(current_node).IsRoot()) {
        /*current_node must then have a parent edge*/
        Edge parent_edge = *dag_.GetNode(current_node).GetParents().begin();
        for (auto mut : parent_edge.GetMutations()){
            muts.insert(mut);
        }
        current_node = parent_edge.GetParent().GetId();
    }
    return muts;
}

template <typename T>
const NodeStorage& NodeView<T>::GetStorage() const {
	return dag_.nodes_.at(id_.value);
}

template <typename T>
inline bool operator==(NodeView<T> lhs, NodeView<T> rhs) {
	return std::addressof(lhs.dag_) == std::addressof(rhs.dag_) &&
		lhs.id_ == rhs.id_;
}
