/*
  HistoryDAG is the main structure that owns node and edge data, and provides
  various queries.

  Populating with data should be performed by first adding all nodes by the
  AddNode() function, then adding all the edges with AddEdge() and finally
  calling BuildConnections().

  NodeId and EdgeId are strongly typed wrappers around size_t, and data is
  stored internally by the order of its IDs.

  GetNodes() and GetEdges() returns a view into the corresponding elements,
  ordered by id.

  TraversePreOrder() returns a view into Nodes in pre-order traversal order.

*/
#pragma once

#include <type_traits>
#include <limits>
#include <iterator>
#include <algorithm>
#include <cassert>
#include <vector>
#include <ranges>
#include <string_view>

#include "history_dag_node_storage.hpp"
#include "history_dag_edge_storage.hpp"
#include "counter_map.hpp"

template <typename T, typename Weight>
	concept LeafFunc = std::invocable<T, Node> &&
		std::convertible_to<std::invoke_result_t<T, Node>, Weight>;

template <typename T, typename Weight>
	concept EdgeWeightFunc = std::invocable<T, Edge> &&
		std::convertible_to<std::invoke_result_t<T, Edge>, Weight>;

template <typename T, typename Weight>
	concept AccumFunc = std::invocable<T,
		std::ranges::ref_view<std::vector<Weight>>> &&
		std::convertible_to<std::invoke_result_t<T,
			std::ranges::ref_view<std::vector<Weight>>>, Weight>;

class HistoryDAG {
public:

	using Weight = double;
	using ArbitraryPrecisionInteger = long;
	
	Node AddNode(NodeId id);
	
	Edge AddEdge(EdgeId id, Node parent, Node child, size_t clade);

	void BuildConnections();
	
	inline CollectionOf<Node> auto GetNodes() const;
	inline CollectionOf<MutableNode> auto GetNodes();
	inline CollectionOf<Edge> auto GetEdges() const;
	inline CollectionOf<MutableEdge> auto GetEdges();
	
	Node GetNode(NodeId id) const;
	MutableNode GetNode(NodeId id);
	Edge GetEdge(EdgeId id) const;
	MutableEdge GetEdge(EdgeId id);

	Node GetRoot() const;
	MutableNode GetRoot();

	inline CollectionOf<Node> auto GetLeafs() const;
	inline CollectionOf<MutableNode> auto GetLeafs();

	inline CollectionOf<Node> auto TraversePreOrder() const;
	inline CollectionOf<MutableNode> auto TraversePreOrder();
	inline CollectionOf<Node> auto TraversePostOrder() const;
	inline CollectionOf<MutableNode> auto TraversePostOrder();

	ArbitraryPrecisionInteger CountHistories() const;
	void WriteProtobuf(std::string_view filename) const;
	HistoryDAG SampleHistory() const;
	HistoryDAG FindHistoryByIndex(ArbitraryPrecisionInteger) const;
	bool IsCladeTree() const;
    void Merge(CollectionOf<const HistoryDAG> auto);
    void AddAllAllowedEdges();

	void PostorderHistoryWeightAccumulation(
		LeafFunc<Weight> auto&& leaf_func,
		EdgeWeightFunc<Weight> auto&& edge_weight_func,
		AccumFunc<Weight> auto&& accum_within_clade,
		AccumFunc<Weight> auto&& accum_between_clade);

	CounterMap<Weight> WeightCount(LeafFunc<Weight> auto&& leaf_func,
		EdgeWeightFunc<Weight> auto&& edge_weight_func,
		AccumFunc<Weight> auto&& accum_func);

	Weight OptimalWeightAnnotate(LeafFunc<Weight> auto&& leaf_func,
		EdgeWeightFunc<Weight> auto&& edge_weight_func,
		AccumFunc<Weight> auto&& accum_func,
		AccumFunc<Weight> auto&& optimal_func);

	void TrimOptimalWeight(LeafFunc<Weight> auto&& leaf_func,
		EdgeWeightFunc<Weight> auto&& edge_weight_func,
		AccumFunc<Weight> auto&& accum_func,
		AccumFunc<Weight> auto&& optimal_fun);
	
private:
	template <typename> friend class NodeView;
	template <typename> friend class EdgeView;
	
	std::vector<NodeStorage> nodes_;
	std::vector<EdgeStorage<Weight>> edges_;
	NodeId root_ = {NoId};
	std::vector<NodeId> leafs_;
};

#include "history_dag_node.hpp"
#include "history_dag_edge.hpp"
#include "pre_order_iterator.hpp"
#include "post_order_iterator.hpp"
#include "inl/history_dag_node_inl.hpp"
#include "inl/history_dag_edge_inl.hpp"
#include "inl/history_dag_inl.hpp"
#include "inl/pre_order_iterator_inl.hpp"
#include "inl/post_order_iterator_inl.hpp"
#include "inl/mutation_inl.hpp"
