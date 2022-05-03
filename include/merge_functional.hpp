#include <vector>
#include <functional>
#include <map>
#include <set>

#include "history_dag.hpp"

class Merge;

class NodeLabel2 {
public:

    void AddChildEdge(CollectionOf<Mutation> auto mutations) {
        for (auto& i : mutations) {
            map_[i.GetPosition()] = i.GetParentNucleotide();
        }
    }

    void AddParentEdge(CollectionOf<Mutation> auto mutations) {
        for (auto& i : mutations) {
            map_[i.GetPosition()] = i.GetMutatedNucleotide();
        }
    }

    void AddParent(const NodeLabel2& parent) {
        for (auto& i : parent.map_) {
            map_[i.first] = i.second;
        }
    }

    bool IsDone() const {
        return done_;
    }

    void SetDone() {
        done_ = true;
    }

    std::string ToString() const {
        std::string result;
        for (auto& i : map_) {
            result += std::to_string(i.first.value);
            result += i.second;
        }
        return result;
    }

    bool operator==(const NodeLabel2& rhs) const {
        return map_ == rhs.map_;
    }

    bool operator<(const NodeLabel2& rhs) const {
        return map_ < rhs.map_;
    }

private:
    std::map<MutationPosition, char> map_ = {};
    bool done_ = false;
};

class NodeKey2 {
public:

    NodeKey2() = default;

    NodeKey2(Merge& merge, size_t tree_idx, NodeId node_id);

    bool operator==(const NodeKey2& rhs) const {
        return label_ == rhs.label_ &&
            leaf_sets_ == rhs.leaf_sets_;
    }

    bool operator<(const NodeKey2& rhs) const {
        if (label_ < rhs.label_) return true;
        if (rhs.label_ < label_) return false;
        return leaf_sets_ < rhs.leaf_sets_;
    }

    std::string ToString() const {
        std::string result;
        result += label_.ToString();
        result += " [";
        for (auto& i : leaf_sets_) {
            for (auto& j : i) {
                result += j.ToString();
                result += ", ";
            }
            result += " | ";
        }
        result += "]";
        return result;
    }

private:
    NodeLabel2 label_; //TODO pointer
    std::vector<std::set<NodeLabel2>> leaf_sets_; //TODO pointers
};

class EdgeKey2 {
public:
    EdgeKey2(const NodeKey2& parent, const NodeKey2& child, CladeIdx clade) ://TODO move keys
        parent_{parent}, child_{child}, clade_{clade} {}

    const NodeKey2& GetParent() const {
        return parent_;
    }

    const NodeKey2& GetChild() const {
        return child_;
    }

    CladeIdx GetClade() const {
        return clade_;
    }

    bool operator==(const EdgeKey2& rhs) const {
        return parent_ == rhs.parent_ &&
            child_ == rhs.child_ &&
            clade_ == rhs.clade_;
    }

    bool operator<(const EdgeKey2& rhs) const {
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
    NodeKey2 parent_;
    NodeKey2 child_;
    CladeIdx clade_;
};

class Merge {
public:
    Merge(std::vector<std::reference_wrapper<const HistoryDAG>>&& trees);

    void Run();

    HistoryDAG& GetResult();
    const HistoryDAG& GetResult() const;

    const std::vector<std::set<NodeId>>& GetLeafSet(size_t tree_idx, NodeId node_id);
    const NodeLabel2& GetLabel(size_t tree_idx, NodeId node_id);
    NodeId GetResultNode(const NodeKey2& key, Node input_node);
    const std::vector<std::vector<std::set<NodeId>>>& GetResultLeafSet() const;

private:
    std::vector<std::reference_wrapper<const HistoryDAG>> trees_;
    std::vector<std::vector<NodeLabel2>> all_labels_;
    std::vector<std::vector<std::vector<std::set<NodeId>>>> all_leaf_sets_;
    std::map<NodeKey2, NodeId> result_nodes_;
    std::set<EdgeKey2> result_edges_;
    HistoryDAG result_;
    MutableNode ua_;
    std::vector<std::vector<std::set<NodeId>>> result_leaf_set_;
};

#include "inl/merge_functional_inl.hpp"
