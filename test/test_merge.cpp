#include <iostream>

#include "test_common.hpp"

#include "history_dag_loader.hpp"
#include "hdag_ops.hpp"

static void test_merge() {

    HistoryDAG source = LoadHistoryDAG("data/1final-tree-1.nh1.pb.gz");
    HistoryDAG reference = LoadHistoryDAG("data/1final-tree-1.nh4.pb.gz");
    HistoryDAG merged = reference.Merge(source);

    // ToDOT(merged, std::cout);
   
}

static void run_test() {
	test_merge();
}

[[maybe_unused]] static const auto test_added = add_test({
	run_test, "Merge"});
