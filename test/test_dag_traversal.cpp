#include <iostream>

#include "test_common.hpp"

#include "hdag_ops.hpp"

static void test_pre_order() {
    HistoryDAG dag = GenerateRandomDag({
        "TT", "CC", "GA", "AA", "CA", "GG",
    });

    struct Recurse {
        std::vector<NodeId> operator()(Node node) {
            std::vector<NodeId> result;
            result.push_back(node.GetId());
            for (auto i : node.GetChildren()) {
                std::vector<NodeId> rec = Recurse{}(i.GetChild());
                result.insert(result.end(), rec.begin(), rec.end());
            }
            return result;
        }
    };

    std::vector<NodeId> result;

    for (Node node : dag.TraversePreOrder()) {
        result.push_back(node.GetId());
    }

    assert_equal(Recurse{}(dag.GetRoot()), result, "Pre-order");
}

static void run_test() {
	test_pre_order();
}

[[maybe_unused]] static const auto test_added = add_test({
	run_test, "DAG traversal"});
