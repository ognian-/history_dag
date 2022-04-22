#include <iostream>
#include <fstream>

#include "nlohmann/json.hpp"

#include "test_common.hpp"

#include "history_dag_loader.hpp"
#include "hdag_ops.hpp"
#include "benchmark.hpp"
#include "zlib_stream.hpp"

static void test_merge() {
    std::ifstream in_compressed("data/merge_result.json.gz");
    assert(in_compressed);
    zlib::ZStringBuf zbuf(in_compressed, 1, 128 * 1024 * 1024);
    std::istream in(&zbuf);
    nlohmann::json j;
    in >> j;


    HistoryDAG source = LoadHistoryDAG("data/1final-tree-1.nh1.pb.gz");
    HistoryDAG reference = LoadHistoryDAG("data/1final-tree-1.nh4.pb.gz");
    Benchmark merge_time;
    merge_time.start();
    HistoryDAG merged = reference.Merge(source);
    merge_time.stop();
    std::cout << "\nDAGs merged in " << merge_time.durationMs() << " ms\n";

    // ToDOT(merged, std::cout);   
}

static void run_test() {
	test_merge();
}

[[maybe_unused]] static const auto test_added = add_test({
	run_test, "Merge"});
