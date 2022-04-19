#pragma once

#include "mutation.hpp"

template <typename Weight>
class EdgeStorage {
	template <typename> friend class EdgeView;
	friend class HistoryDAG;

	NodeId parent_;
	NodeId child_;
	size_t clade_ = NoId;
	double probability_ = 0.0;
	Weight weight_ = {};
};
