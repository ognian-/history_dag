#include <iostream>

#include "test_common.hpp"
#include "history_dag.hpp"
#include "hdag_ops.hpp"
#include "merge.hpp"

[[maybe_unused]]
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

[[maybe_unused]]
static void test_compact_genome() {
    HistoryDAG lhs = GenerateBinaryTree();

    AddMutations(lhs);

    HistoryDAG merged = Merge({lhs});
    ToDOT(merged, LeafSet(merged), std::cout);
}

static void run_test() {
    // std::cout << "\n";
	// test_compact_genome();
}

[[maybe_unused]] static const auto test_added = add_test({
	run_test, "Compact genome"});
