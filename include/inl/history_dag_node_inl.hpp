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
const NodeStorage& NodeView<T>::GetStorage() const {
	return dag_.nodes_.at(id_.value);
}

template <typename T>
inline bool operator==(NodeView<T> lhs, NodeView<T> rhs) {
	return std::addressof(lhs.dag_) == std::addressof(rhs.dag_) &&
		lhs.id_ == rhs.id_;
}
