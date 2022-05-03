#include <cassert>

NodeKey2::NodeKey2(Merge& merge, size_t tree_idx, NodeId node_id) :
    label_{merge.GetLabel(tree_idx, node_id)} {
    for (auto& i : merge.GetLeafSet(tree_idx, node_id)) {
        std::set<NodeLabel2> set;
        for (auto j : i) set.insert(merge.GetLabel(tree_idx, j));
        leaf_sets_.push_back(set);
    }
}

Merge::Merge(std::vector<std::reference_wrapper<const HistoryDAG>>&& trees) :
        trees_{trees}, ua_{result_.AddNode({0})} {
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
            NodeId parent = edge.GetParent().GetId(), child = edge.GetChild().GetId();
            const EdgeKey2 key =
                {{*this, tree_idx, parent},
                {*this, tree_idx, child},
                edge.GetClade()};
            auto i = result_edges_.find(key);
            if (i == result_edges_.end()) {
                result_edges_.insert(key);
                result_.AddEdge({result_.GetEdges().size()},
                    GetResultNode(key.GetParent(), edge.GetParent()),
                    GetResultNode(key.GetChild(), edge.GetChild()),
                    key.GetClade())
                    .SetMutations(edge.GetMutations());
            }
        }
    }
    result_.BuildConnections();
    result_leaf_set_.reserve(result_.GetNodes().size());
    for (Node i : result_.TraversePostOrder()) {
        for (auto clade : i.GetClades()) {
            for (auto child : clade) {
                auto& cs = GetOrInsert(GetOrInsert(result_leaf_set_, i.GetId()),
                    child.GetClade().value);
                if (child.IsLeaf()) {
                    cs.insert(child.GetChild().GetId());
                } else {
                    auto& clades =
                        result_leaf_set_.at(child.GetChild().GetId().value);
                    for (auto& leafs : clades) {
                        cs.insert(std::begin(leafs), std::end(leafs));
                    }
                }
            }
        }
    }
}

const std::vector<std::set<NodeId>>& Merge::GetLeafSet(size_t tree_idx, NodeId node_id) {
    assert(tree_idx < all_leaf_sets_.size());
    assert(node_id.value < all_leaf_sets_.at(tree_idx).size());
    std::vector<std::set<NodeId>>& result =
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
        auto& leafs = GetLeafSet(tree_idx, child.GetId());
        if (leafs.size() > result.size()) result.resize(leafs.size());
        for (size_t i = 0; i < result.size(); ++i) {
            result.at(i).insert(leafs.at(i).begin(), leafs.at(i).end());
        }
    }
    return result;
}

const NodeLabel2& Merge::GetLabel(size_t tree_idx, NodeId node_id) {
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
        result.AddParent(GetLabel(tree_idx, parent));
        result.AddParentEdge(node.GetFirstParent().GetMutations());
    }
    result.SetDone();
    return result;
}

NodeId Merge::GetResultNode(const NodeKey2& key, Node input_node) {
    auto i = result_nodes_.find(key);
    NodeId id;
    if (i == result_nodes_.end()) {
        id = result_.AddNode({result_.GetNodes().size()}).GetId();
        result_nodes_.insert(i, {key, id});

        if (input_node.IsRoot()) {
            if (result_edges_.insert({NodeKey2{}, key, {0}}).second) {
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

const std::vector<std::vector<std::set<NodeId>>>& Merge::GetResultLeafSet() const {
    return result_leaf_set_;
}

HistoryDAG& Merge::GetResult() { return result_; }

const HistoryDAG& Merge::GetResult() const { return result_; }
