#include <iostream>

#include "test_common.hpp"

#include "history_dag_loader.hpp"
#include "merge_dags.hpp"
#include "hdag_ops.hpp"

static void test_merge() {
    HistoryDAG source_dag = LoadHistoryDAG("data/1final-tree-1.nh1.pb.gz");
    HistoryDAG reference_dag = LoadHistoryDAG("data/1final-tree-1.nh4.pb.gz");

    // std::vector<char> universal_ancestral_sequence = {'C', 'A'};
    // MergeDAGs merge{universal_ancestral_sequence};

    // auto result = merge(dag1, dag2);

    // ToDOT(result, std::cout);
}

static void run_test() {
	test_merge();
}

[[maybe_unused]] static const auto test_added = add_test({
	run_test, "Merge DAGs"});
