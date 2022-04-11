#pragma once

#include <random>
#include <ostream>

#include "history_dag.hpp"

HistoryDAG GenerateRandomDag(std::vector<std::string> leaf_sequences);

HistoryDAG GenerateBinaryTree();

void PrintDag(HistoryDAG& dag);

void ToDOT(HistoryDAG& dag, std::ostream& out);
