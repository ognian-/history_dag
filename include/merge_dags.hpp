#pragma once

#include "history_dag.hpp"

class MergeDAGs {
public:

    HistoryDAG operator()(HistoryDAG& lhs, HistoryDAG& rhs);

};
