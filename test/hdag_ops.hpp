#pragma once

#include <random>
#include <ostream>

#include "merge.hpp"
#include "leaf_set.hpp"

HistoryDAG GenerateRandomDag(std::vector<std::string> leaf_sequences);

HistoryDAG GenerateBinaryTree();

void PrintDag(HistoryDAG& dag);

void ToDOT(HistoryDAG& dag, std::ostream& out);
void ToDOT(HistoryDAG& dag, const LeafSet& leaf_set, std::ostream& out);
