#include <iostream>

#include "test_common.hpp"

#include "history_dag_loader.hpp"
#include "hdag_ops.hpp"
#include "benchmark.hpp"

#include <valgrind/callgrind.h>//XXX

static void test_merge() {

    HistoryDAG source = LoadHistoryDAG("data/1final-tree-1.nh1.pb.gz");
    HistoryDAG reference = LoadHistoryDAG("data/1final-tree-1.nh4.pb.gz");
    Benchmark merge_time;
    CALLGRIND_START_INSTRUMENTATION;
    merge_time.start();
    HistoryDAG merged = reference.Merge(source);
    merge_time.stop();
    CALLGRIND_STOP_INSTRUMENTATION;
    std::cout << "\nDAGs merged in " << merge_time.durationMs() << " ms\n";

    // ToDOT(merged, std::cout);   
}

static void run_test() {
	test_merge();
}

[[maybe_unused]] static const auto test_added = add_test({
	run_test, "Merge"});
