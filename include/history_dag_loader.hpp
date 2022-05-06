#pragma once

#include <string>
#include <unordered_map>

#include "merge_functional.hpp"

[[nodiscard]] HistoryDAG LoadHistoryDAGFromProtobufGZ(const std::string& path);

[[nodiscard]] HistoryDAG LoadHistoryDAGFromJsonGZ(const std::string& path, std::string& refseq);

[[nodiscard]] std::vector<NodeLabel> LoadLabelsJsonGZ(const std::string& path);
