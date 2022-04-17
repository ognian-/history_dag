#pragma once

#include "mutation.hpp"

template <typename Weight>
class EdgeStorage {
	template <typename> friend class EdgeView;
	friend class HistoryDAG;

	void AddMutation(const Mutation& mutation);
    void ClearMutations();

	NodeId parent_;
	NodeId child_;
	size_t clade_ = NoId;
	std::vector<Mutation> mutations_;
    double probability_ = 0.0;
	Weight weight_ = {};
};

///////////////////////////////////////////////////////////////////////////////

template <typename Weight>
void EdgeStorage<Weight>::AddMutation(const Mutation& mutation) {
    mutations_.push_back(mutation);
}

template <typename Weight>
void EdgeStorage<Weight>::ClearMutations() {
    mutations_.clear();
}
