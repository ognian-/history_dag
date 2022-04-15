#pragma once

#include "history_dag_common.hpp"

class EdgeStorage {
	template <typename> friend class EdgeView;
	friend class HistoryDAG;
	NodeId parent_;
	NodeId child_;
	size_t clade_;
};
