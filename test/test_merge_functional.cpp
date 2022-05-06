#include "merge_functional.hpp"

#include <iostream>
#include <valgrind/callgrind.h>

#include "test_common.hpp"
#include "history_dag_loader.hpp"
#include "benchmark.hpp"

[[maybe_unused]]
static std::string ToString4(Node node, const std::vector<std::map<MutationPosition, char>>& cgs) {
	// if (node.IsRoot()) return "p";
	std::string result;
	
    result += std::to_string(node.GetId().value);
    result += ":\\n";

    size_t num = 0;
    for (auto [pos, mut] : cgs.at(node.GetId().value)) {
        result += std::to_string(pos.value);
        result += mut;
        result += ++num % 3 == 0 ? "\\n" : " ";
    }

	return result;
}

[[maybe_unused]]
static void ToDOT4(HistoryDAG& dag, const std::vector<std::map<MutationPosition, char>>& cgs, std::ostream& out) {
	out << "digraph {\n";
	for (auto i : dag.GetEdges()) {
		std::string parent = ToString4(i.GetParent(), cgs);
		std::string child = ToString4(i.GetChild(), cgs);
		out << "  \"" << parent << "\" -> \"" << child << "\"\n";
	}
	out << "}\n";
}

[[maybe_unused]]
static void test_real() {
    std::string refseq0, refseq1, refseq2, refseq_correct;

    HistoryDAG correct_result =
        LoadHistoryDAGFromJsonGZ("data/littledag.json.gz", refseq_correct);
    HistoryDAG tree0 =
        LoadHistoryDAGFromJsonGZ("data/final-tree-1.nh1.pb.json.gz", refseq0);
    HistoryDAG tree1 =
        LoadHistoryDAGFromJsonGZ("data/final-tree-2.nh1.pb.json.gz", refseq1);
    HistoryDAG tree2 =
        LoadHistoryDAGFromJsonGZ("data/final-tree-3.nh1.pb.json.gz", refseq2);
    
    assert_equal(refseq0, refseq1, "Reference sequence");
    assert_equal(refseq1, refseq2, "Reference sequence");
    assert_equal(refseq2, refseq_correct, "Reference sequence");

    std::vector<std::vector<NodeLabel>> labels;
    labels.resize(3);

    labels[0] = LoadLabelsJsonGZ("data/final-tree-1.nh1.pb.json.gz");
    labels[1] = LoadLabelsJsonGZ("data/final-tree-2.nh1.pb.json.gz");
    labels[2] = LoadLabelsJsonGZ("data/final-tree-3.nh1.pb.json.gz");

    Benchmark merge_time;
    merge_time.start();
    Merge merged(refseq0, {tree0, tree1, tree2}, labels);
    merged.Run();
    merge_time.stop();
    std::cout << "\nDAGs merged in " << merge_time.durationMs() << " ms\n";
    
    std::cout << "Tree0 nodes: " << tree0.GetNodes().size() << "\n";
    std::cout << "Tree0 edges: " << tree0.GetEdges().size() << "\n\n";

    std::cout << "Tree1 nodes: " << tree1.GetNodes().size() << "\n";
    std::cout << "Tree1 edges: " << tree1.GetEdges().size() << "\n\n";

    std::cout << "Tree2 nodes: " << tree2.GetNodes().size() << "\n";
    std::cout << "Tree2 edges: " << tree2.GetEdges().size() << "\n\n";

    std::cout << "Correct nodes: " << correct_result.GetNodes().size() << "\n";
    std::cout << "Correct edges: " << correct_result.GetEdges().size() << "\n\n";

    std::cout << "Merged nodes: " << merged.GetResult().GetNodes().size() << "\n";
    std::cout << "Merged edges: " << merged.GetResult().GetEdges().size() << "\n\n";

    // ToDOT4(merged.GetResult(), merged.GetResultCompactGenomes(), std::cout);
}

static void run_test() {
    std::cout << "\n";
    test_real();
}

[[maybe_unused]] static const auto test_added = add_test({
	run_test, "Merge functional"});
