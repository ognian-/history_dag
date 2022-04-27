#pragma once

#include <set>

#include "history_dag_common.hpp"

class LeafSet {
public:
    inline explicit LeafSet(const HistoryDAG& dag);

    inline CollectionOfCollections<Node> auto GetLeafs(NodeId node) const;

private:
    const HistoryDAG& dag_;
    std::vector<std::vector<std::set<NodeId>>> leaf_sets_;
};
