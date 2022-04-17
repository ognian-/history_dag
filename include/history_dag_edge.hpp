/*
  Edge is a lightweight view object into the internal edge storage of a
  HistoryDAG. It is meant to be cheaply passed by value, and behaves as a
  reference into raw storage that conveniently enriches it's API.
*/
#pragma once

#include <optional>

#include "history_dag_common.hpp"

class HistoryDAG;

template <typename T>
class EdgeView {
public:
	constexpr static const bool is_mutable = std::is_same_v<T, HistoryDAG&>;
	EdgeView(T dag, EdgeId id);
	operator Edge() const;
	const HistoryDAG& GetDAG() const;
	EdgeId GetId() const;
	Node GetParent() const;
	Node GetChild() const;
	size_t GetClade() const;
	CollectionOf<const Mutation> auto GetMutations() const;
    double GetProbability() const;
	const auto& GetWeight() const;
	std::optional<EdgeView> FindNextSibling() const;

	void AddMutation(const Mutation& mutation);
    void ClearMutations();
private:
	template <typename U> friend bool operator==(EdgeView<U>, EdgeView<U>);
	const auto& GetStorage() const;

	T dag_;
	const EdgeId id_;
};
