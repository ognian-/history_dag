#include <iostream>

#include "test_common.hpp"

#include "history_dag_loader.hpp"
#include "hdag_ops.hpp"
#include "benchmark.hpp"


static void test_loading() {
    Benchmark load_time;
    load_time.start();
    HistoryDAG dag = LoadHistoryDAG("public-latest.all.masked.pb.gz");
    load_time.stop();
    std::cout << "\nDAG loaded in " << load_time.durationMs() << " ms\n";
    // ToDOT(dag, std::cout);
}

static void run_test() {
	test_loading();
}

[[maybe_unused]] static const auto test_added = add_test({
	run_test, "Loading hDAG"});
