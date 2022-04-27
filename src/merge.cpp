#include <unordered_map>
#include <set>

#include "merge.hpp"
#include "compact_genome.hpp"

size_t Hash(size_t hash, MutationPosition position, char parent,
    char mutated) {
    hash = HashCombine(hash, position.value);
    hash = HashCombine(hash, parent);
    hash = HashCombine(hash, mutated);
    return hash;
}

size_t Hash(size_t hash, CollectionOf<Mutation> auto mutations) {
    for (auto i : mutations) {
        hash = Hash(hash, i.GetPosition(), i.GetParentNucleotide(),
            i.GetMutatedNucleotide());
    }
    return hash;
}

size_t Hash(size_t hash,
    const std::pair<CompactGenome, std::vector<std::set<CompactGenome>>>& pair) {
    hash = Hash(hash, pair.first.GetMutations());
    for (auto i : pair.second | std::views::join) hash = Hash(hash, i.GetMutations());
    return hash;
}

static const auto RootMutations = [](const Mutation& mutation) -> Mutation {
    return {mutation.GetPosition(), mutation.GetParentNucleotide(),
        mutation.GetParentNucleotide()};
};

class MergeData {
public:
    explicit MergeData(const HistoryDAG& dag) : dag_{dag}, leaf_set_{dag} {
        cgs_.reserve(dag_.GetNodes().size());
        
        for (auto i : dag_.TraversePreOrder()) {
            auto& cg = GetOrInsert(cgs_, i.GetNode().GetId());
            if (i.GetNode().IsRoot()) {
                cg.SetMutations(i.GetEdge().GetMutations() |
                    std::views::transform(RootMutations), true);
                continue;
            }
            cg.SetMutations(i.GetEdge().GetMutations());
            Node parent = i.GetEdge().GetParent();
            if (not parent.IsRoot()) {
                cg.AddMutations(cgs_.at(parent.GetFirstParent().GetParent()
                    .GetId().value).GetMutations());
            }
        }
    }

    auto BuildMap() const {
        auto HashFn = [this]
        (const std::pair<CompactGenome, std::vector<std::set<CompactGenome>>>& pair) {
            return Hash(0, pair);
        };

        auto EqFn = [this]
        (const std::pair<CompactGenome, std::vector<std::set<CompactGenome>>>& lhs,
        const std::pair<CompactGenome, std::vector<std::set<CompactGenome>>>& rhs) {
            return lhs == rhs;
        };

        std::unordered_map<std::pair<CompactGenome, std::vector<std::set<CompactGenome>>>,
            NodeId, decltype(HashFn), decltype(EqFn)> map{
                dag_.GetNodes().size(), HashFn, EqFn};

        for (NodeId i : dag_.GetNodes() |
            std::views::transform(Transform::GetId)) {
            map[BuildKey(i)] = i;
        }
        return map;
    }

    std::pair<CompactGenome, std::vector<std::set<CompactGenome>>>
    BuildKey(NodeId id) const {
        std::vector<std::set<CompactGenome>> leafs;
        for (auto i : leaf_set_.GetLeafs(id)) {
            std::set<CompactGenome> set;
            for (auto j : i) set.insert(cgs_.at(j.GetId().value));
            leafs.push_back(set);
        }
        return {cgs_.at(id.value), leafs};
    }

    const LeafSet& GetLeafSet() const {
        return leaf_set_;
    }

private:
    const HistoryDAG& dag_;
    std::vector<CompactGenome> cgs_;
    LeafSet leaf_set_;
};

HistoryDAG Merge(const HistoryDAG& reference, const HistoryDAG& source) {
    HistoryDAG result = reference;
    MergeData ref_data{reference};
    MergeData src_data{source};

    auto map = ref_data.BuildMap();
    MutableNode ua = result.AddNode({result.GetNodes().size()});
    for (auto iter : source.TraversePostOrder()) {
        Node i = iter.GetNode();
        if (i.IsRoot()) {
            result.AddEdge({result.GetEdges().size()}, ua, i, {0})
                .SetMutations(iter.GetEdge().GetMutations() |
                    std::views::transform(RootMutations));
        }
        auto key = src_data.BuildKey(i.GetId());
        auto i_ref = map.find(key);
        NodeId corresponding_node;
        if (i_ref == map.end()) {
            MutableNode new_node = result.AddNode({result.GetNodes().size()});
            map[key] = new_node.GetId();
            corresponding_node = new_node.GetId();
        } else {
            corresponding_node = i_ref->second;
        }
        for (Edge child_edge : i.GetChildren()) {
            Node child = child_edge.GetChild();
            auto corresponding_child =
                map.find(src_data.BuildKey(child.GetId()));
            bool have_edge = false;
            for (Edge edge : result.GetEdges()) {
                if (edge.GetParent().GetId() == corresponding_node and
                    edge.GetChild().GetId() == corresponding_child->second and
                    edge.GetClade() == child_edge.GetClade()) {
                    have_edge = true;
                    break;
                }
            }
            if (not have_edge) {
                result.AddEdge({result.GetEdges().size()},
                corresponding_node, corresponding_child->second, child_edge.GetClade())
                .SetMutations(child_edge.GetMutations());
            }
        }
    }
    result.BuildConnections();
    return result;
}
