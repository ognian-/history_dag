#pragma once

#include <string>

#include "history_dag.hpp"

[[nodiscard]] HistoryDAG LoadHistoryDAGFromProtobufGZ(const std::string& path);

[[nodiscard]] HistoryDAG LoadHistoryDAGFromJsonGZ(const std::string& path);
