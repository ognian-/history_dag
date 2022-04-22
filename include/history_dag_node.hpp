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
	using NodeType = std::conditional_t<is_mutable, MutableNode, Node>;
	using EdgeType = std::conditional_t<is_mutable, MutableEdge, Edge>;
	NodeView(T dag, NodeId id);
	operator Node() const;
	const HistoryDAG& GetDAG() const;
	NodeId GetId() const;
	CollectionOf<char> auto GetSequence() const;
	auto GetParents() const -> CollectionOf<EdgeType> auto;
	auto GetClades() const -> CollectionOfCollections<EdgeType> auto;
	auto GetChildren() const -> CollectionOf<EdgeType> auto;
	auto GetLeafsBelow() const -> CollectionOf<NodeType> auto;
	EdgeType GetSingleParent() const;
	EdgeType GetFirstParent() const;
	std::string_view GetLabel() const;
	void SetLabel(std::string_view label);
	void CopyConnections(Node node);//XXX
	bool IsRoot() const;
	bool IsLeaf() const;
	void AddParentEdge(Edge edge);
	void AddChildEdge(Edge edge);
	void RemoveParentEdge(Edge edge);
	auto BuildMutsRelReference() const;
private:
	template <typename U> friend bool operator==(NodeView<U>, NodeView<U>);
	template <typename> friend class NodeView;
	auto& GetStorage() const;
	T dag_;
	const NodeId id_;
};
