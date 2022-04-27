#pragma once

#include <random>
#include <ostream>

#include "merge.hpp"

HistoryDAG GenerateRandomDag(std::vector<std::string> leaf_sequences);

HistoryDAG GenerateBinaryTree();

void PrintDag(HistoryDAG& dag);

void ToDOT(HistoryDAG& dag, std::ostream& out);
void ToDOT(HistoryDAG& dag, const std::vector<std::vector<std::set<NodeId>>>& clade_sets,
    std::ostream& out);
