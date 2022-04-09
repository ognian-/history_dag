#pragma once

#include "history_dag_common.hpp"

class EdgeStorage {
public:
	NodeId parent_;
	NodeId child_;
	size_t clade_;
};
