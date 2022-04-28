#pragma once

#include <vector>
#include <functional>

#include "history_dag.hpp"

HistoryDAG Merge(
    const std::vector<std::reference_wrapper<const HistoryDAG>>& trees);
