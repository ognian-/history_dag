#include <unordered_map>

#include "merge.hpp"
#include "compact_genome.hpp"

std::unordered_map<NodeId, CompactGenome> ComputeCompactGenomes(
    const HistoryDAG& dag) {
    std::unordered_map<NodeId, CompactGenome> result;
    result.reserve(dag.GetNodes().size());
    for (Node i : dag.TraversePreOrder()) {
        if (i.IsRoot()) continue;
        result[i.GetId()].AddMutations(i.GetFirstParent().GetMutations());
    }
    return result;
}

HistoryDAG Merge(const HistoryDAG& reference, const HistoryDAG& source) {
    HistoryDAG result = reference;
    
    ComputeCompactGenomes(source);

    return result;
}
