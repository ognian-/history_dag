#include "merge_functional.hpp"

#include <iostream>
#include <valgrind/callgrind.h>

#include "test_common.hpp"
#include "history_dag_loader.hpp"
#include "benchmark.hpp"

static void AddMutations(HistoryDAG& dag) {
    std::vector<Mutation> muts;

    muts = {{{0}, 'A', 'C'}, {{1}, 'T', 'A'}};
    dag.GetEdge({0}).SetMutations(muts | std::views::all);

    muts = {{{0}, 'A', 'T'}, {{1}, 'T', 'G'}};
    dag.GetEdge({1}).SetMutations(muts | std::views::all);

    muts = {{{0}, 'T', 'G'}, {{1}, 'A', 'C'}};
    dag.GetEdge({2}).SetMutations(muts | std::views::all);

    muts = {{{0}, 'T', 'C'}, {{1}, 'A', 'T'}};
    dag.GetEdge({3}).SetMutations(muts | std::views::all);


    muts = {{{0}, 'G', 'A'}, {{1}, 'A', 'G'}};
    dag.GetEdge({4}).SetMutations(muts | std::views::all);

    muts = {{{0}, 'G', 'T'}, {{1}, 'A', 'C'}};
    dag.GetEdge({5}).SetMutations(muts | std::views::all);

    muts = {{{0}, 'C', 'G'}, {{1}, 'C', 'A'}};
    dag.GetEdge({6}).SetMutations(muts | std::views::all);

    muts = {{{0}, 'C', 'A'}, {{1}, 'C', 'T'}};
    dag.GetEdge({7}).SetMutations(muts | std::views::all);


    muts = {{{0}, 'G', 'A'}, {{1}, 'C', 'T'}};
    dag.GetEdge({8}).SetMutations(muts | std::views::all);

    muts = {{{0}, 'G', 'T'}, {{1}, 'C', 'A'}};
    dag.GetEdge({9}).SetMutations(muts | std::views::all);



    muts = {{{0}, 'C', 'G'}, {{1}, 'T', 'A'}};
    dag.GetEdge({10}).SetMutations(muts | std::views::all);

    muts = {{{0}, 'C', 'C'}, {{1}, 'T', 'C'}};
    dag.GetEdge({11}).SetMutations(muts | std::views::all);


    muts = {{{0}, 'T', 'G'}, {{1}, 'A', 'C'}};
    dag.GetEdge({12}).SetMutations(muts | std::views::all);

    muts = {{{0}, 'T', 'C'}, {{1}, 'A', 'T'}};
    dag.GetEdge({13}).SetMutations(muts | std::views::all);
}

static HistoryDAG GenerateBinaryTree() {
    HistoryDAG dag;
    for (size_t i = 0; i < 15; ++i) dag.AddNode({i});
    dag.AddEdge({0}, dag.GetNode({8}), dag.GetNode({0}), {0});
    dag.AddEdge({1}, dag.GetNode({8}), dag.GetNode({1}), {1});
    dag.AddEdge({2}, dag.GetNode({9}), dag.GetNode({2}), {0});
    dag.AddEdge({3}, dag.GetNode({9}), dag.GetNode({3}), {1});
    dag.AddEdge({4}, dag.GetNode({10}), dag.GetNode({4}), {0});
    dag.AddEdge({5}, dag.GetNode({10}), dag.GetNode({5}), {1});
    dag.AddEdge({6}, dag.GetNode({11}), dag.GetNode({6}), {0});
    dag.AddEdge({7}, dag.GetNode({11}), dag.GetNode({7}), {1});
    dag.AddEdge({8}, dag.GetNode({12}), dag.GetNode({8}), {0});
    dag.AddEdge({9}, dag.GetNode({12}), dag.GetNode({9}), {1});
    dag.AddEdge({10}, dag.GetNode({13}), dag.GetNode({10}), {0});
    dag.AddEdge({11}, dag.GetNode({13}), dag.GetNode({11}), {1});
    dag.AddEdge({12}, dag.GetNode({14}), dag.GetNode({12}), {0});
    dag.AddEdge({13}, dag.GetNode({14}), dag.GetNode({13}), {1});
    dag.BuildConnections();
	return dag;
}

static std::string ToString(Node node, const std::vector<std::set<NodeId>>& leaf_set) {
	if (node.IsRoot()) return "p";
	std::string result;
	
	for (auto& i : node.GetFirstParent().GetMutations()) {
		result += std::to_string(i.GetPosition().value);
		result += i.GetMutatedNucleotide();
	}

	result += " [";
	bool remove_clade_sep = false;
	for (auto clade : leaf_set) {
		bool remove_leaf_sep = false;
		for (auto leaf : clade) {
			for (auto& i : node.GetDAG().GetNode(leaf).GetFirstParent().GetMutations()) {
				result += std::to_string(i.GetPosition().value);
				result += i.GetMutatedNucleotide();
			}
			result += ",";
			remove_leaf_sep = true;
		}
		if (remove_leaf_sep) result.erase(result.size() - 1);
		result += "|";
		remove_clade_sep = true;
	}
	if (remove_clade_sep) result.erase(result.size() - 1);
	result += "]";

	return result;
}

static void ToDOT(HistoryDAG& dag, const std::vector<std::vector<std::set<NodeId>>>& leaf_sets, std::ostream& out) {
	out << "digraph {\n";
	for (auto i : dag.GetEdges()) {
		std::string parent = ToString(i.GetParent(), leaf_sets.at(i.GetParent().GetId().value));
		std::string child = ToString(i.GetChild(), leaf_sets.at(i.GetChild().GetId().value));
		out << "  \"" << parent << "\" -> \"" << child << "\"\n";
	}
	out << "}\n";
}

static std::vector<std::vector<std::set<NodeId>>> GetLeafset(const HistoryDAG& dag) {
    std::vector<std::vector<std::set<NodeId>>> leaf_sets;
    leaf_sets.reserve(dag.GetNodes().size());
    for (Node i : dag.TraversePostOrder()) {
        for (auto clade : i.GetClades()) {
            for (auto child : clade) {
                auto& cs = GetOrInsert(GetOrInsert(leaf_sets, i.GetId()),
                    child.GetClade().value);
                if (child.IsLeaf()) {
                    cs.insert(child.GetChild().GetId());
                } else {
                    auto& clades =
                        leaf_sets.at(child.GetChild().GetId().value);
                    for (auto& leafs : clades) {
                        cs.insert(std::begin(leafs), std::end(leafs));
                    }
                }
            }
        }
    }
    return leaf_sets;
}

[[maybe_unused]]
static void test_merge_functional() {
    HistoryDAG lhs = GenerateBinaryTree();

    AddMutations(lhs);

    Merge merge{{lhs}};

    merge.Run();

    ToDOT(merge.GetResult(), GetLeafset(merge.GetResult()), std::cout);
}

[[maybe_unused]]
static void test_real() {
    HistoryDAG correct_result =
        LoadHistoryDAGFromJsonGZ("data/merge_result.json.gz");
    HistoryDAG source =
        LoadHistoryDAGFromProtobufGZ("data/1final-tree-1.nh4.pb.gz");
    HistoryDAG reference =
        LoadHistoryDAGFromProtobufGZ("data/1final-tree-1.nh1.pb.gz");
    Benchmark merge_time;
    merge_time.start();
    CALLGRIND_START_INSTRUMENTATION;
    Merge merged{{reference, source}};
    merged.Run();
    CALLGRIND_STOP_INSTRUMENTATION;
    merge_time.stop();
    std::cout << "\nDAGs merged in " << merge_time.durationMs() << " ms\n";

    std::cout << "Source nodes: " << source.GetNodes().size() << "\n";
    std::cout << "Source edges: " << source.GetEdges().size() << "\n\n";

    std::cout << "Reference nodes: " << reference.GetNodes().size() << "\n";
    std::cout << "Reference edges: " << reference.GetEdges().size() << "\n\n";

    std::cout << "Correct nodes: " << correct_result.GetNodes().size() << "\n";
    std::cout << "Correct edges: " << correct_result.GetEdges().size() << "\n\n";

    std::cout << "Merged nodes: " << merged.GetResult().GetNodes().size() << "\n";
    std::cout << "Merged edges: " << merged.GetResult().GetEdges().size() << "\n";  
}

static void run_test() {
    std::cout << "\n";
	// test_merge_functional();
    test_real();
}

[[maybe_unused]] static const auto test_added = add_test({
	run_test, "Merge functional"});
