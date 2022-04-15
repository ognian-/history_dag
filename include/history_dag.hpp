/*
  HistoryDAG is the main structure that owns node and edge data, and provides
  various queries.

  Populating with data should be performed by first adding all nodes by the
  AddNode() function, then adding all the edges with AddEdge() and finally
  calling Finalize().

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

#include "history_dag_node_storage.hpp"
#include "history_dag_edge_storage.hpp"

class HistoryDAG {
public:
	
	template <typename Sequence>
	Node AddNode(NodeId id, Sequence&& sequence);
	
	Edge AddEdge(EdgeId id, Node parent, Node child, size_t clade);

	void Finalize();
	
	inline CollectionOf<Node> auto GetNodes() const;
	inline CollectionOf<Edge> auto GetEdges() const;
	
	Node GetNode(NodeId id) const;
	Edge GetEdge(EdgeId id) const;

	Node GetRoot() const;

	inline CollectionOf<Node> auto GetLeafs() const;

	inline CollectionOf<Node> auto TraversePreOrder() const;
	inline CollectionOf<Node> auto TraversePostOrder() const;
	
private:
	template <typename> friend class NodeView;
	template <typename> friend class EdgeView;
	
	std::vector<NodeStorage> nodes_;
	std::vector<EdgeStorage> edges_;
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
