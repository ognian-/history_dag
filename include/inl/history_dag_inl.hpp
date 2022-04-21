#include <unordered_map>
#include <numeric>

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

inline std::string MutStr(Node node);//XXX

HistoryDAG HistoryDAG::Merge(const HistoryDAG& source) {
	HistoryDAG result = *this;
	std::unordered_map<size_t, std::vector<NodeId>> map;
	std::unordered_map<std::string, NodeId> new_node_muts;//XXX
	for (Node node : TraversePostOrder()) {
		if (node.IsRoot()) continue;
		map[HashOfNode(node)].push_back(node.GetId());
	}
	for (Node node : source.TraversePostOrder()) {
		if (node.IsRoot()) continue;
		const size_t hash = HashOfNode(node);
		NodeId corresponding_node_id = {NoId};
		auto i = map.find(hash);
		if (i != map.end()) {
			for (auto j : i->second |
				std::views::transform([&result](NodeId node_id) {
					return Node{result, node_id};
				})) {
					if (Equal(j, node)) {
						corresponding_node_id = j.GetId();
						break;
					}
				}
		}
		if (corresponding_node_id.value == NoId) {
			MutableNode new_node = result.AddNode({result.GetNodes().size()});
			new_node.SetLabel(node.GetLabel());
			new_node.CopyConnections(node);//XXX
			new_node_muts[MutStr(node)] = new_node.GetId();//XXX
			corresponding_node_id = new_node.GetId();
			map[hash].push_back(corresponding_node_id);
		}
		for (Node child : node.GetChildren() |
			std::views::transform(Transform::GetChild)) {
			const size_t hash = HashOfNode(child);
			NodeId corresponding_child_id = {NoId};
			auto i = map.find(hash);
			assert(i != map.end());
			
			for (auto j : i->second |
				std::views::transform([&result](NodeId node_id) {
					return Node{result, node_id};
				})) {
					if (Equal(j, child)) {
						corresponding_child_id = j.GetId();
						break;
					}
					if (new_node_muts.contains(MutStr(child))) {//XXX
						corresponding_child_id = new_node_muts[MutStr(child)];
						break;
					}
				}

			assert(corresponding_child_id.value != NoId);
			bool have_edge = false;
			for (Edge edge : result.GetEdges()) {
				if (edge.GetParent().GetId() == corresponding_node_id &&
					edge.GetChild().GetId() == corresponding_child_id) {
					have_edge = true;
					break;
				}
			}
			if (not have_edge) {
				result.AddEdge({result.GetEdges().size()},
					result.GetNode(corresponding_node_id),
					result.GetNode(corresponding_child_id),
					0).SetMutations(
						(*result.GetNode(corresponding_child_id)
							.GetParents().begin()).GetMutations()
					);
			}
		}
	}

	return result;
}

CounterMap<HistoryDAG::Weight> HistoryDAG::WeightCount(
	LeafFunc<HistoryDAG::Weight> auto&& /*leaf_func*/,
	EdgeWeightFunc<HistoryDAG::Weight> auto&& /*edge_weight_func*/,
	AccumFunc<Weight> auto&& /*accum_func*/) {
	// TODO implement
	return {};
}

size_t HistoryDAG::HashOf(CollectionOf<Mutation> auto mutations) {
	static auto MutationHash = [](const Mutation& mut) -> size_t {
		size_t hash = std::hash<size_t>{}(mut.GetPosition()) ^
			std::hash<char>{}(mut.GetReferenceNucleotide()) ^
			std::hash<char>{}(mut.GetParentNucleotide());
		auto mutated = mut.GetMutatedNucleotide();
		hash = std::accumulate(std::begin(mutated), std::end(mutated), hash,
			HashCombine);
		return hash;
	};
	if (std::begin(mutations) == std::end(mutations)) return 0;
	if (++std::begin(mutations) == std::end(mutations))
		return MutationHash(*std::begin(mutations));

	size_t hash = 0;
	for (auto i : mutations | std::views::transform(MutationHash)) {
		hash = HashCombine(hash, i);
	}
	return hash;
}
