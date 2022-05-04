#include <vector>
#include <functional>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <cassert>

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
        for (auto mutation : mutations) {
            MutationPosition pos = mutation.GetPosition();
            char nuc = mutation.GetMutatedNucleotide();
            auto i = map_.find(pos);
            if (i == map_.end()) {
                map_[pos] = nuc;
            } else {
                if (i->second == nuc) {
                    map_.erase(i);
                } else {
                    i->second = nuc;
                }
            }
        }
    }

    void AddParent(const NodeLabel2& parent) {
        for (auto [pos, nuc] : parent.map_) {
            map_[pos] = nuc;
        }
    }

    bool IsDone() const {
        return hash_ != NoId;
    }

    void SetDone() {
        size_t hash = 0;
        for (auto [pos, nuc] : map_) {
            hash = HashCombine(hash, pos.value);
            hash = HashCombine(hash, nuc);
        }
        hash_ = hash;
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
    friend struct std::hash<NodeLabel2>;
    std::map<MutationPosition, char> map_ = {};
    size_t hash_ = NoId;
};

template<> struct std::hash<NodeLabel2> {
    std::size_t operator()(const NodeLabel2& label) const noexcept {
        return label.hash_;
    }
};

class NodeKey2 {
public:

    NodeKey2() = default;

    NodeKey2(Merge& merge, size_t tree_idx, NodeId node_id);

    bool operator==(const NodeKey2& rhs) const {
        if (not label_ and not rhs.label_) return true;
        if (not label_ or not rhs.label_) return false;
        return *label_ == *rhs.label_ &&
            leaf_sets_ == rhs.leaf_sets_;
    }

    bool operator<(const NodeKey2& rhs) const {
        assert(label_ and rhs.label_);
        if (*label_ < *rhs.label_) return true;
        if (*rhs.label_ < *label_) return false;
        return leaf_sets_ < rhs.leaf_sets_;
    }

    std::string ToString() const {
        if (not label_) return "p";
        std::string result;
        result += label_->ToString();
        result += " [";
        for (auto& i : leaf_sets_) {
            for (auto& j : i) {
                result += j->ToString();
                result += ", ";
            }
            result += " | ";
        }
        result += "]";
        return result;
    }

private:
    friend struct std::hash<NodeKey2>;
    const NodeLabel2* label_ = nullptr;
    std::vector<std::set<const NodeLabel2*>> leaf_sets_;
};

template<> struct std::hash<NodeKey2> {
    std::size_t operator()(const NodeKey2& key) const noexcept {
        size_t hash = 0;
        if (not key.label_) return hash;
        hash = HashCombine(hash, std::hash<NodeLabel2>{}(*key.label_));
        for (auto& i : key.leaf_sets_) {
            for (auto& j : i) {
                hash = HashCombine(hash, std::hash<NodeLabel2>{}(*j));
            }
        }
        return hash;
    }
};

class EdgeKey2 {
public:
    EdgeKey2(NodeKey2&& parent, NodeKey2&& child, CladeIdx clade) :
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
    friend struct std::hash<EdgeKey2>;
    NodeKey2 parent_;
    NodeKey2 child_;
    CladeIdx clade_;
};

template<> struct std::hash<EdgeKey2> {
    std::size_t operator()(const EdgeKey2& key) const noexcept {
        size_t hash = 0;
        hash = HashCombine(hash, std::hash<NodeKey2>{}(key.parent_));
        hash = HashCombine(hash, std::hash<NodeKey2>{}(key.child_));
        hash = HashCombine(hash, key.clade_.value);
        return hash;
    }
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

private:
    std::vector<std::reference_wrapper<const HistoryDAG>> trees_;
    std::vector<std::vector<NodeLabel2>> all_labels_;
    std::vector<std::vector<std::vector<std::set<NodeId>>>> all_leaf_sets_;
    std::unordered_map<NodeKey2, NodeId> result_nodes_;
    std::unordered_set<EdgeKey2> result_edges_;
    HistoryDAG result_;
    MutableNode ua_;
};

#include "inl/merge_functional_inl.hpp"
