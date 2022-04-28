#include <set>
#include <map>

#include "merge.hpp"

class NodeLabel {
public:
    NodeLabel() : label_{"p"} {}

    explicit NodeLabel(Node node) {
        if (!node.IsRoot()) {
		for (auto& i : node.GetFirstParent().GetMutations()) {
                label_ += i.GetMutatedNucleotide();
            }
        } else {
            for (auto& i : (*node.GetChildren().begin()).GetMutations()) {
                label_ += i.GetParentNucleotide();
            }
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

    NodeKey(Node node, decltype(std::declval<LeafSet>().GetLeafs({})) leafs) :
        label_{node} {
        for (auto i : leafs) {
            std::set<NodeLabel> set;
            for (auto j : i) set.insert(NodeLabel{j});
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
            child_ == rhs.child_;
    }

    bool operator<(const EdgeKey& rhs) const {
        if (parent_ < rhs.parent_) return true;
        if (rhs.parent_ < parent_) return false;
        if (child_ < rhs.child_) return true;
        if (rhs.child_ < child_) return false;
        return clade_ < rhs.clade_;
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

HistoryDAG Merge(
    const std::vector<std::reference_wrapper<const HistoryDAG>>& trees) {
    std::vector<LeafSet> leafs;
    for (auto i : trees) leafs.emplace_back(i.get());
    std::map<NodeKey, NodeId> nodes;
    std::set<EdgeKey> edges;
    HistoryDAG result;

    MutableNode ua = result.AddNode({0});

    for (size_t i = 0; i < trees.size(); ++i) {
        for (Node node : trees[i].get().GetNodes()) {
            const NodeKey key{node, leafs[i].GetLeafs(node.GetId())};
            auto i = nodes.find(key);
            NodeId id;
            if (i == nodes.end()) {
                id = result.AddNode({result.GetNodes().size()}).GetId();
                nodes[key] = id;
            } else {
                id = nodes.at(key);
            }
            if (node.IsRoot()) {
                if (edges.insert({NodeKey{}, key, {0}}).second) {
                    result.AddEdge({result.GetEdges().size()},
                        ua.GetId(), id, {0})
                        .SetMutations((*node.GetChildren().begin())
                        .GetMutations() | std::views::transform(RootMutations));
                }
            }
        }
    }

    for (size_t i = 0; i < trees.size(); ++i) {
        for (Edge edge : trees[i].get().GetEdges()) {
            const EdgeKey key =
                {{edge.GetParent(), leafs[i].GetLeafs(edge.GetParent().GetId())},
                {edge.GetChild(), leafs[i].GetLeafs(edge.GetChild().GetId())},
                edge.GetClade()};
            if (key.GetParent() == key.GetChild()) continue;

            auto i = edges.find(key);
            if (i == edges.end()) {
                edges.insert(key);
                result.AddEdge({result.GetEdges().size()},
                    nodes.at(key.GetParent()),
                    nodes.at(key.GetChild()),
                    key.GetClade())
                    .SetMutations(edge.GetMutations());
            }
        }
    }

    result.BuildConnections();
    
    return result;
}
