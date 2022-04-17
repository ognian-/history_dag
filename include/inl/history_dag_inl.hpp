template <typename Sequence>
Node HistoryDAG::AddNode(NodeId id, Sequence&& sequence) {
	assert(id.value != NoId);
	auto& storage = GetOrInsert(nodes_, id);
	storage.sequence_.clear();
	std::copy(sequence.begin(), sequence.end(),
		std::back_inserter(storage.sequence_));
	return {*this, id};
}

CollectionOf<Node> auto HistoryDAG::GetNodes() const {
	return nodes_ | std::views::transform(
		[this, idx = size_t{}](const NodeStorage&) mutable {
			return Node{*this, {idx++}};
		});
}

CollectionOf<MutableNode> auto HistoryDAG::GetNodes() {
	return nodes_ | std::views::transform(
		[this, idx = size_t{}](NodeStorage&) mutable {
			return MutableNode{*this, {idx++}};
		});
}

CollectionOf<Edge> auto HistoryDAG::GetEdges() const {
	return edges_ | std::views::transform(
		[this, idx = size_t{}](const EdgeStorage<Weight>&) mutable {
			return Edge{*this, {idx++}};
		});
}

CollectionOf<MutableEdge> auto HistoryDAG::GetEdges() {
	return edges_ | std::views::transform(
		[this, idx = size_t{}](EdgeStorage<Weight>&) mutable {
			return MutableEdge{*this, {idx++}};
		});
}

CollectionOf<Node> auto HistoryDAG::GetLeafs() const {
	return leafs_ | std::views::transform([this](NodeId node_id) {
			return Node{*this, node_id};
		});
}

CollectionOf<MutableNode> auto HistoryDAG::GetLeafs() {
	return leafs_ | std::views::transform([this](NodeId node_id) {
			return MutableNode{*this, node_id};
		});
}

CollectionOf<Node> auto HistoryDAG::TraversePreOrder() const {
	return std::ranges::subrange(PreOrderIterator{GetRoot()},
		PreOrderIterator<Node>{});
}

CollectionOf<MutableNode> auto HistoryDAG::TraversePreOrder() {
	return std::ranges::subrange(PreOrderIterator{GetRoot()},
		PreOrderIterator<MutableNode>{});
}

CollectionOf<Node> auto HistoryDAG::TraversePostOrder() const {
	return std::ranges::subrange(PostOrderIterator{GetRoot()},
		PostOrderIterator<Node>{});
}

CollectionOf<MutableNode> auto HistoryDAG::TraversePostOrder() {
	return std::ranges::subrange(PostOrderIterator{GetRoot()},
		PostOrderIterator<MutableNode>{});
}
