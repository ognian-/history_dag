#pragma once

#include <vector>
#include <tuple>
#include <map>

#include "history_dag.hpp"

template <typename Seq>
class CompactGenome {
public:
    using Site = std::decay_t<decltype(*(std::declval<Seq>().begin()))>;

    template <typename Seq2>
    CompactGenome(const Seq& parent, const Seq2& child);

    CompactGenome() = default;
    CompactGenome(const CompactGenome&) = default;

private:
    std::vector<std::tuple<size_t, Site, Site>> differences_;
};

class CladeSet {
public:

    explicit CladeSet(Node node);

    CladeSet() = default;
    CladeSet(const CladeSet&) = default;

private:
    std::vector<NodeId> leafs_;
};

template <typename Seq>
class MergeDAGs {
public:
    MergeDAGs(Seq&& universal_ancestral_sequence);

    HistoryDAG& operator()(HistoryDAG& source, HistoryDAG& reference);

private:
    Seq universal_ancestral_sequence_;
    std::map<NodeId, std::pair<CompactGenome<Seq>, CladeSet>> map_;
};

///////////////////////////////////////////////////////////////////////////////

template <typename Seq>
template <typename Seq2>
CompactGenome<Seq>::CompactGenome(const Seq& parent, const Seq2& child) {
    assert(parent.size() == child.size());
    size_t site_id = 0;
    auto child_site = child.begin();
    for (auto parent_site : parent) {
        if (parent_site != *child_site) {
            differences_.push_back({site_id, parent_site, *child_site});
        }
        ++site_id;
        ++child_site;
    }
}

template <typename Seq>
MergeDAGs<Seq>::MergeDAGs(Seq&& universal_ancestral_sequence) :
    universal_ancestral_sequence_{universal_ancestral_sequence} {}

template <typename Seq>
HistoryDAG& MergeDAGs<Seq>::operator()(HistoryDAG&,
    HistoryDAG& reference) {
    for (Node node : reference.TraversePreOrder()) {
        CompactGenome<Seq> compact_genome{universal_ancestral_sequence_,
            node.GetSequence()};
            
        CladeSet clade_set{node};

        map_[node.GetId()] = std::make_pair(compact_genome, clade_set);
    }

    return reference;
}
