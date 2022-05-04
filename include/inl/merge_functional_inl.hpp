Merge::Merge(std::vector<std::reference_wrapper<const HistoryDAG>>&& trees) :
        trees_{std::move(trees)}, ua_{result_.AddNode({0})} {
    all_labels_.resize(trees_.size());
    all_leaf_sets_.resize(trees_.size());
    for (size_t tree_idx = 0; tree_idx < trees_.size(); ++tree_idx) {
        size_t size = trees_.at(tree_idx).get().GetNodes().size() + 1;
        all_labels_.at(tree_idx).resize(size);
        all_leaf_sets_.at(tree_idx).resize(size);
    }
}

void Merge::Run() {
    for (size_t tree_idx = 0; tree_idx < trees_.size(); ++tree_idx) {
        for (Edge edge : trees_.at(tree_idx).get().GetEdges()) {
            MakeResultEdgeTask(tree_idx, edge.GetId());
        }
    }
    result_.BuildConnections();
}

const std::vector<std::unordered_set<NodeId>>& Merge::GetLeafSetTask(size_t tree_idx, NodeId node_id) {
    assert(tree_idx < all_leaf_sets_.size());
    assert(node_id.value < all_leaf_sets_.at(tree_idx).size());
    std::vector<std::unordered_set<NodeId>>& result =
        all_leaf_sets_.at(tree_idx).at(node_id.value);
    if (not result.empty()) return result;
    Node node = trees_.at(tree_idx).get().GetNode(node_id);
    if (node.IsLeaf()) return result;
    for (auto child_edge : node.GetChildren()) {
        Node child = child_edge.GetChild();
        if (child.IsLeaf()) {
            GetOrInsert(result, child_edge.GetClade()).insert(child.GetId());
            continue;
        }
        auto& leafs = GetLeafSetTask(tree_idx, child.GetId());
        if (leafs.size() > result.size()) result.resize(leafs.size());
        for (size_t i = 0; i < leafs.size(); ++i) {
            result.at(i).insert(leafs.at(i).begin(), leafs.at(i).end());
        }
    }
    return result;
}

const NodeLabel2& Merge::GetLabelTask(size_t tree_idx, NodeId node_id) {
    assert(tree_idx < all_labels_.size());
    assert(node_id.value < all_labels_.at(tree_idx).size());
    NodeLabel2& result = all_labels_.at(tree_idx).at(node_id.value);
    if (result.IsDone()) return result;
    Node node = trees_.at(tree_idx).get().GetNode(node_id);
    if (node.IsRoot()) {
        for (auto child : node.GetChildren()) {
            result.AddChildEdge(child.GetMutations());
        }
    } else {
        NodeId parent = node.GetFirstParent().GetParent().GetId();
        result.AddParent(GetLabelTask(tree_idx, parent));
        result.AddParentEdge(node.GetFirstParent().GetMutations());
    }
    result.SetDone();
    return result;
}

NodeId Merge::GetResultNodeTask(const NodeKey2& key, Node input_node) {
    auto i = result_nodes_.find(key);
    NodeId id;
    if (i == result_nodes_.end()) {
        id = result_.AddNode({result_.GetNodes().size()}).GetId();
        result_nodes_.emplace_hint(i, key.Copy(), id);

        if (input_node.IsRoot()) {
            if (result_edges_.emplace(NodeKey2::UA(), key.Copy(), CladeIdx{0}).second) {
                result_.AddEdge({result_.GetEdges().size()},
                    ua_.GetId(), id, {0})
                    .SetMutations((*input_node.GetChildren().begin()).GetMutations() |
                        std::views::transform([](const Mutation& mutation) -> Mutation {
                            return {mutation.GetPosition(), mutation.GetParentNucleotide(),
                                mutation.GetParentNucleotide()};
                        }));
            }
        }
    } else {
        id = i->second;
    }
    
    return id;
}

void Merge::MakeResultEdgeTask(size_t tree_idx, EdgeId edge_id) {
    Edge edge = trees_.at(tree_idx).get().GetEdge(edge_id);
    NodeId parent = edge.GetParent().GetId(), child = edge.GetChild().GetId();
    EdgeKey2 key =
        {MakeNodeKeyTask(tree_idx, parent),
        MakeNodeKeyTask(tree_idx, child),
        edge.GetClade()};
    auto i = result_edges_.find(key);
    if (i == result_edges_.end()) {
        result_.AddEdge({result_.GetEdges().size()},
            GetResultNodeTask(key.GetParent(), edge.GetParent()),
            GetResultNodeTask(key.GetChild(), edge.GetChild()),
            key.GetClade())
            .SetMutations(edge.GetMutations());
        result_edges_.emplace_hint(i, std::move(key));
    }
}

NodeKey2 Merge::MakeNodeKeyTask(size_t tree_idx, NodeId node_id) {
    auto& leaf_set = GetLeafSetTask(tree_idx, node_id);
    std::vector<std::unordered_set<const NodeLabel2*>> leaf_sets;
    leaf_sets.reserve(leaf_set.size());
    for (auto& i : leaf_set) {
        std::unordered_set<const NodeLabel2*> set;
        for (auto j : i) set.insert(&GetLabelTask(tree_idx, j));
        leaf_sets.emplace_back(std::move(set));
    }
    return {&GetLabelTask(tree_idx, node_id), std::move(leaf_sets)};
}

HistoryDAG& Merge::GetResult() { return result_; }

const HistoryDAG& Merge::GetResult() const { return result_; }
