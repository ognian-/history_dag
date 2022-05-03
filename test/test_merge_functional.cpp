#include "merge_functional.hpp"

#include <iostream>

#include "test_common.hpp"

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

static void test_merge_functional() {
    HistoryDAG lhs = GenerateBinaryTree();

    AddMutations(lhs);

    Merge merge{{lhs}};

    merge.Run();

    ToDOT(merge.GetResult(), merge.GetResultLeafSet(), std::cout);
}

static void run_test() {
    std::cout << "\n";
	test_merge_functional();
}

[[maybe_unused]] static const auto test_added = add_test({
	run_test, "Merge functional"});
