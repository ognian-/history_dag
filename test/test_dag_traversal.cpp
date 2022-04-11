#include <iostream>

#include "test_common.hpp"

#include "hdag_ops.hpp"

static void test_pre_order() {
    HistoryDAG dag = GenerateRandomDag({
        "TT", "CC", "GA", "AA", "CA", "GG",
    });

    ToDOT(dag, std::cout);

    for (auto i : dag.GetNodesPreOrder()) {
        std::cout << "  Node: " << i.GetId().value << "\n";
    }
}

static void run_test() {
	test_pre_order();
}

[[maybe_unused]] static const auto test_added = add_test({
	run_test, "DAG traversal"});
