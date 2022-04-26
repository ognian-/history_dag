#pragma once

#include <tuple>

#include "history_dag.hpp"

HistoryDAG Merge(const HistoryDAG& reference, const HistoryDAG& source);
