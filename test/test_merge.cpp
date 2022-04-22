#include <iostream>

#include "test_common.hpp"

#include "history_dag_loader.hpp"
#include "hdag_ops.hpp"
#include "benchmark.hpp"

static void test_merge() {
    HistoryDAG correct_result = LoadHistoryDAGFromJsonGZ("data/merge_result.json.gz");
    HistoryDAG source = LoadHistoryDAGFromProtobufGZ("data/1final-tree-1.nh1.pb.gz");
    HistoryDAG reference = LoadHistoryDAGFromProtobufGZ("data/1final-tree-1.nh4.pb.gz");
    Benchmark merge_time;
    merge_time.start();
    HistoryDAG merged = reference.Merge(source);
    merge_time.stop();
    std::cout << "\nDAGs merged in " << merge_time.durationMs() << " ms\n";

    std::cout << "Source nodes: " << source.GetNodes().size() << "\n";
    std::cout << "Source edges: " << source.GetEdges().size() << "\n\n";

    std::cout << "Reference nodes: " << reference.GetNodes().size() << "\n";
    std::cout << "Reference edges: " << reference.GetEdges().size() << "\n\n";

    std::cout << "Correct nodes: " << correct_result.GetNodes().size() << "\n";
    std::cout << "Correct edges: " << correct_result.GetEdges().size() << "\n\n";

    std::cout << "Merged nodes: " << merged.GetNodes().size() << "\n";
    std::cout << "Merged edges: " << merged.GetEdges().size() << "\n";  
}

static void run_test() {
	test_merge();
}

[[maybe_unused]] static const auto test_added = add_test({
	run_test, "Merge"});
