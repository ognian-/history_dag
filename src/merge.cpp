#include <set>
#include <map>
#include <optional>

#include "merge.hpp"

class CompactGenome {
public:

    std::optional<char> Get(MutationPosition position) const {
        auto i = map_.find(position);
        if (i == map_.end()) return std::nullopt;
        return i->second;
    }

    void SetCG(CollectionOf<Mutation> auto muts, const CompactGenome& parent) {
        map_ = parent.map_;
        for (Mutation i : muts) {
            MutationPosition position = i.GetPosition();
            char mutation = i.GetMutatedNucleotide();
            map_[position] = mutation;
        }
    }

    const std::map<MutationPosition, char>& GetMutations() const {
        return map_;
    }

    std::string ToString() const {
        std::string result;
        for (auto mut : GetMutations()) {
            result += std::to_string(mut.first.value);
            result += mut.second;
            result += ',';
        }
        if (not result.empty()) result.erase(result.size() - 1);
        return result;
    }

private:
    std::map<MutationPosition, char> map_;
};

class NodeLabel {
public:
    NodeLabel() : label_{"p"} {}

    explicit NodeLabel(const CompactGenome& cg) {
        for (auto i : cg.GetMutations()) {
            label_ += std::to_string(i.first.value);
            label_ += i.second;
        }
    }

    const std::string& GetString() const {
        return label_;
    }

    bool operator==(const NodeLabel& rhs) const {
        return label_ == rhs.label_;
    }

    bool operator<(const NodeLabel& rhs) const {
        return label_ < rhs.label_;
    }

private:
    std::string label_;
};

class NodeKey {
public:

    NodeKey() = default;

    NodeKey(const CompactGenome& cg, const std::vector<CompactGenome>& cgs,
        decltype(std::declval<LeafSet>().GetLeafs({})) leafs) :
        label_{cg} {
        for (auto i : leafs) {
            std::set<NodeLabel> set;
            for (auto j : i) set.insert(NodeLabel{cgs.at(j.GetId().value)});
            leaf_sets_.push_back(set);
        }
    }

    bool operator==(const NodeKey& rhs) const {
        return label_ == rhs.label_ &&
            leaf_sets_ == rhs.leaf_sets_;
    }

    bool operator<(const NodeKey& rhs) const {
        if (label_ < rhs.label_) return true;
        if (rhs.label_ < label_) return false;
        return leaf_sets_ < rhs.leaf_sets_;
    }

    std::string ToString() const {
        std::string result;
        result += label_.GetString();
        result += " [";
        for (auto& i : leaf_sets_) {
            for (auto& j : i) {
                result += j.GetString();
                result += ", ";
            }
            result += " | ";
        }
        result += "]";
        return result;
    }

private:
    NodeLabel label_;
    std::vector<std::set<NodeLabel>> leaf_sets_;
};

class EdgeKey {
public:
    EdgeKey(NodeKey parent, NodeKey child, CladeIdx clade) :
        parent_{parent}, child_{child}, clade_{clade} {}

    const NodeKey& GetParent() const {
        return parent_;
    }

    const NodeKey& GetChild() const {
        return child_;
    }

    CladeIdx GetClade() const {
        return clade_;
    }

    bool operator==(const EdgeKey& rhs) const {
        return parent_ == rhs.parent_ &&
            child_ == rhs.child_ &&
            clade_ == rhs.clade_;
    }

    bool operator<(const EdgeKey& rhs) const {
        if (parent_ < rhs.parent_) return true;
        if (rhs.parent_ < parent_) return false;
        if (child_ < rhs.child_) return true;
        if (rhs.child_ < child_) return false;
        return clade_ < rhs.clade_;
    }

    std::string ToString() const {
        std::string result;
        result += "\"";
        result += parent_.ToString();
        result += "\" -> \"";
        result += child_.ToString();
        result += "\"";
        return result;
    }

private:
    NodeKey parent_;
    NodeKey child_;
    CladeIdx clade_;
};

static const auto RootMutations = [](const Mutation& mutation) -> Mutation {
    return {mutation.GetPosition(), mutation.GetParentNucleotide(),
        mutation.GetParentNucleotide()};
};

static void ComputeCG(const HistoryDAG& tree, std::vector<CompactGenome>& cgs) {

    cgs.resize(tree.GetNodes().size());
    for (auto iter : tree.TraversePreOrder()) {
        auto& cg = GetOrInsert(cgs, iter.GetEdge().GetChild().GetId());
        if (iter.IsRoot()) {
            cg.SetCG(iter.GetEdge().GetMutations(), {});
        } else {
            cg.SetCG(iter.GetEdge().GetMutations(),
                cgs.at(iter.GetNode().GetFirstParent().GetParent().GetId().value));
        }
    }
}

HistoryDAG Merge(
    const std::vector<std::reference_wrapper<const HistoryDAG>>& trees) {
    std::vector<std::vector<CompactGenome>> cgs;
    std::vector<LeafSet> leafs;
    for (auto i : trees) leafs.emplace_back(i.get());
    std::map<NodeKey, NodeId> nodes;
    std::set<EdgeKey> edges;
    HistoryDAG result;


    MutableNode ua = result.AddNode({0});

    for (size_t tree_idx = 0; tree_idx < trees.size(); ++tree_idx) {
        const HistoryDAG& tree = trees[tree_idx].get();
        auto& tree_cgs = GetOrInsert(cgs, tree_idx);

        ComputeCG(tree, tree_cgs);

        for (Node node : tree.GetNodes()) {
            const NodeKey key{tree_cgs.at(node.GetId().value), tree_cgs, leafs[tree_idx].GetLeafs(node.GetId())};
            auto i = nodes.find(key);
            NodeId id;
            if (i == nodes.end()) {
                id = result.AddNode({result.GetNodes().size()}).GetId();
                nodes[key] = id;
            } else {
                assert(tree_idx != 0);
                id = nodes.at(key);
            }
            if (node.IsRoot()) {
                if (edges.insert({NodeKey{}, key, {0}}).second) {
                    result.AddEdge({result.GetEdges().size()},
                        ua.GetId(), id, {0})
                        .SetMutations((*node.GetChildren().begin()).GetMutations() | std::views::transform(RootMutations));
                }
            }
        }
    }

    for (size_t tree_idx = 0; tree_idx < trees.size(); ++tree_idx) {
        const HistoryDAG& tree = trees[tree_idx].get();
        auto& tree_cgs = cgs.at(tree_idx);
        for (Edge edge : tree.GetEdges()) {
            NodeId parent = edge.GetParent().GetId(), child = edge.GetChild().GetId();
            const EdgeKey key =
                {{tree_cgs.at(parent.value), tree_cgs, leafs[tree_idx].GetLeafs(parent)},
                {tree_cgs.at(child.value), tree_cgs, leafs[tree_idx].GetLeafs(child)},
                edge.GetClade()};
            auto i = edges.find(key);
            if (i == edges.end()) {
                edges.insert(key);
                result.AddEdge({result.GetEdges().size()},
                    nodes.at(key.GetParent()),
                    nodes.at(key.GetChild()),
                    key.GetClade())
                    .SetMutations(tree_cgs.at(child.value).GetMutations() | std::views::transform([](auto& mut) {
                        return Mutation{mut.first, mut.second, mut.second};
                    }));
                    // .SetMutations(edge.GetMutations());
            }
        }
    }

    result.BuildConnections();
    
    return result;
}
