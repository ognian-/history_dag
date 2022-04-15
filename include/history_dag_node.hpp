/*
  Node is a lightweight view object into the internal node storage of a
  HistoryDAG. It is meant to be cheaply passed by value, and behaves as a
  reference into raw storage that conveniently enriches it's API.
*/
#pragma once

#include "history_dag_common.hpp"

class NodeStorage;

class Mutation;

template <typename T>
class NodeView {
public:
	constexpr static const bool is_mutable = std::is_same_v<T, HistoryDAG&>;
	NodeView(T dag, NodeId id);
	operator Node() const;
	const HistoryDAG& GetDAG() const;
	NodeId GetId() const;
	CollectionOf<char> auto GetSequence() const;
	CollectionOf<Edge> auto GetParents() const;
	CollectionOfCollections<Edge> auto GetClades() const;
	CollectionOf<Edge> auto GetChildren() const;
	CollectionOf<Node> auto GetLeafsBelow() const;
	bool IsRoot() const;
	bool IsLeaf() const;
	void AddParentEdge(Edge edge);
	void AddChildEdge(Edge edge);
	void RemoveParentEdge(Edge edge);
	CollectionOf<Mutation> auto BuildMutsRelReference() const;
private:
	template <typename U> friend bool operator==(NodeView<U>, NodeView<U>);
	const NodeStorage& GetStorage() const;
	T dag_;
	const NodeId id_;
};
