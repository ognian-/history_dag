#include <unordered_map>

#include "merge.hpp"
#include "compact_genome.hpp"

HistoryDAG Merge(const HistoryDAG& reference, const HistoryDAG& source) {
    HistoryDAG result = reference;
    
    std::vector<CompactGenome> source_cgs;
    source_cgs.reserve(source.GetNodes().size());
    for (auto i : source.TraversePreOrder()) {
        if (i.IsRoot()) continue;
        GetOrInsert(source_cgs, i.GetId())
            .AddMutations(i.GetFirstParent().GetMutations());
    }

    return result;
}
