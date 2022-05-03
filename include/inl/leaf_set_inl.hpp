LeafSet::LeafSet(const HistoryDAG& dag) : dag_{dag} {
    leaf_sets_.reserve(dag.GetNodes().size());
    for (Node i : dag_.TraversePostOrder()) {
        for (auto clade : i.GetClades()) {
            for (auto child : clade) {
                auto& cs = GetOrInsert(GetOrInsert(leaf_sets_, i.GetId()),
                    child.GetClade().value);
                if (child.IsLeaf()) {
                    cs.insert(child.GetChild().GetId());
                } else {
                    auto& clades =
                        leaf_sets_.at(child.GetChild().GetId().value);
                    for (auto& leafs : clades) {
                        cs.insert(std::begin(leafs), std::end(leafs));
                    }
                }
            }
        }
    }
}

CollectionOfCollections<Node> auto LeafSet::GetLeafs(NodeId node) const {
    return leaf_sets_.at(node.value)
        | std::views::transform([this](const std::set<NodeId>& leafs) {
            return leafs
                | std::views::transform([this](NodeId leaf_id) -> Node {
                    return {dag_, leaf_id};
                });
        });
}
