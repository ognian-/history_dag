#include <iostream>

#include "test_common.hpp"

#include "hdag_ops.hpp"
#include "pre_order_iterator.hpp"

static void test_pre_order() {
    HistoryDAG dag = GenerateRandomDag({
        "TT", "CC", "GA", "AA", "CA", "GG",
    });

    ToDOT(dag, std::cout);

    PreOrderIterator poi{dag.GetRoot()};

    for (size_t i = 0; i < dag.GetNodes().size() - 1; ++i) {
        size_t val = (*poi).GetId().value;
        std::cout << "  Node: " << val << "\n";
        ++poi;
    }
}

static void run_test() {
	test_pre_order();
}

[[maybe_unused]] static const auto test_added = add_test({
	run_test, "DAG traversal"});
