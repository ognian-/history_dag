#include <unordered_map>
#include <set>

#include "merge.hpp"
#include "compact_genome.hpp"

size_t Hash(size_t hash, MutationPosition position, char parent,
    char reference) {
    hash = HashCombine(hash, position.value);
    hash = HashCombine(hash, parent);
    hash = HashCombine(hash, reference);
    return hash;
}

size_t Hash(size_t hash, CollectionOf<Mutation> auto mutations) {
    for (auto i : mutations) {
        hash = Hash(hash, i.GetPosition(), i.GetParentNucleotide(),
            i.GetReferenceNucleotide());
    }
    return hash;
}

size_t Hash(size_t hash,
    const std::pair<CompactGenome, std::set<CompactGenome>>& pair) {
    hash = Hash(hash, pair.first.GetMutations());
    for (auto& i : pair.second) hash = Hash(hash, i.GetMutations());
    return hash;
}

class MergeData {
public:
    explicit MergeData(const HistoryDAG& dag) : dag_{dag} {
        cgs_.reserve(dag_.GetNodes().size());
        clade_sets_.reserve(dag_.GetNodes().size());

        for (auto i : dag_.TraversePreOrder()) {
            auto& cg = GetOrInsert(cgs_, i.GetNode().GetId());
            cg.AddMutations(i.GetEdge().GetMutations());
            Node parent = i.GetEdge().GetParent();
            if (not parent.IsRoot()) {
                cg.AddMutations(parent.GetFirstParent().GetMutations());
            }
        }

        for (Node i : dag_.TraversePostOrder()) {
            for (auto child : i.GetChildren() |
                std::views::transform(Transform::GetChild)) {
                auto& cs = GetOrInsert(clade_sets_, i.GetId());
                if (child.IsLeaf()) {
                    cs.insert(child.GetId());
                } else {
                    auto& leafs = clade_sets_.at(child.GetId().value);
                    cs.insert(std::begin(leafs), std::end(leafs));
                }
            }
        }
    }

    auto BuildMap() const {
        auto HashFn = [this]
        (const std::pair<CompactGenome, std::set<CompactGenome>>& pair) {
            return Hash(0, pair);
        };

        auto EqFn = [this]
        (const std::pair<CompactGenome, std::set<CompactGenome>>& lhs,
        const std::pair<CompactGenome, std::set<CompactGenome>>& rhs) {
            return lhs == rhs;
        };

        std::unordered_map<std::pair<CompactGenome, std::set<CompactGenome>>,
            NodeId, decltype(HashFn), decltype(EqFn)> map{
                dag_.GetNodes().size(), HashFn, EqFn};

        for (NodeId i : dag_.GetNodes() |
            std::views::transform(Transform::GetId)) {
            std::set<CompactGenome> leafs;
            auto view = clade_sets_.at(i.value) |
                std::views::transform([this](NodeId leaf_id) {
                    return cgs_.at(leaf_id.value);
                });
            leafs.insert(view.begin(), view.end());
            map[{cgs_.at(i.value), leafs}] = i;
        }
        return map;
    }

    std::pair<CompactGenome, std::set<CompactGenome>>
    BuildKey(NodeId id) const {
        std::set<CompactGenome> leafs;
        auto view = clade_sets_.at(id.value) |
            std::views::transform([this](NodeId id) {
            return cgs_.at(id.value);
        });
        leafs.insert(view.begin(), view.end());
        return {cgs_.at(id.value), leafs};
    }

private:
    const HistoryDAG& dag_;
    std::vector<CompactGenome> cgs_;
    std::vector<std::set<NodeId>> clade_sets_;
};

HistoryDAG Merge(const HistoryDAG& reference, const HistoryDAG& source) {
    HistoryDAG result = reference;
    MergeData ref_data{reference};
    MergeData src_data{source};

    auto map = ref_data.BuildMap();
    for (Node i : source.TraversePostOrder()) {
        if (i.IsRoot()) continue;
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
        for (Node child : i.GetChildren() |
            std::views::transform(Transform::GetChild)) {
            auto corresponding_child =
                map.find(src_data.BuildKey(child.GetId()));
            bool have_edge = false;
            for (Edge edge : result.GetEdges()) {
                if (edge.GetParent().GetId() == corresponding_node and
                    edge.GetChild().GetId() == corresponding_child->second) {
                    have_edge = true;
                    break;
                }
            }
            if (not have_edge) result.AddEdge({result.GetEdges().size()},
                corresponding_node, corresponding_child->second, {0});
        }
    }

    return result;
}
