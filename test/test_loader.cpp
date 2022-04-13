#include <iostream>

#include "test_common.hpp"

#include "history_dag_loader.hpp"
#include "benchmark.hpp"


static void test_loading() {
    Benchmark load_time;
    load_time.start();
    HistoryDAG dag = LoadHistoryDAG("data/1final-tree-1.nh1.pb.gz");
    load_time.stop();
    // std::cout << "\nDAG loaded in " << load_time.durationMs() << " ms\n";
}

static void run_test() {
	test_loading();
}

[[maybe_unused]] static const auto test_added = add_test({
	run_test, "Loading hDAG"});
