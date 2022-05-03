#include <iostream>

#include "test_common.hpp"

#include "history_dag_loader.hpp"
#include "merge.hpp"
#include "hdag_ops.hpp"
#include "benchmark.hpp"

[[maybe_unused]]
static void test_simple() {
    HistoryDAG lhs;
    for (size_t i = 0; i < 7; ++i) lhs.AddNode({i});
    std::vector<Mutation> muts0{{{0}, 'C', 'A'}, {{1}, 'G', 'A'}};
    std::vector<Mutation> muts1{{{0}, 'C', 'C'}, {{1}, 'G', 'T'}};
    std::vector<Mutation> muts2{{{0}, 'C', 'T'}, {{1}, 'A', 'A'}};
    std::vector<Mutation> muts3{{{0}, 'C', 'C'}, {{1}, 'A', 'C'}};
    std::vector<Mutation> muts4{{{0}, 'C', 'C'}, {{1}, 'A', 'G'}};
    std::vector<Mutation> muts5{{{0}, 'C', 'C'}, {{1}, 'A', 'A'}};
    lhs.AddEdge({0}, {4}, {0}, {0}).SetMutations(muts0 | std::views::all);
    lhs.AddEdge({1}, {4}, {1}, {1}).SetMutations(muts1 | std::views::all);
    lhs.AddEdge({2}, {5}, {2}, {0}).SetMutations(muts2 | std::views::all);
    lhs.AddEdge({3}, {5}, {3}, {1}).SetMutations(muts3 | std::views::all);
    lhs.AddEdge({4}, {6}, {4}, {0}).SetMutations(muts4 | std::views::all);
    lhs.AddEdge({5}, {6}, {5}, {1}).SetMutations(muts5 | std::views::all);
    lhs.BuildConnections();

    HistoryDAG rhs;
    for (size_t i = 0; i < 7; ++i) rhs.AddNode({i});
    muts0 = {{{0}, 'C', 'A'}, {{1}, 'A', 'A'}};
    muts1 = {{{0}, 'C', 'C'}, {{1}, 'A', 'T'}};
    muts2 = {{{0}, 'C', 'T'}, {{1}, 'G', 'A'}};
    muts3 = {{{0}, 'C', 'C'}, {{1}, 'G', 'C'}};
    muts4 = {{{0}, 'C', 'C'}, {{1}, 'A', 'A'}};
    muts5 = {{{0}, 'C', 'C'}, {{1}, 'A', 'G'}};
    rhs.AddEdge({0}, {4}, {0}, {0}).SetMutations(muts0 | std::views::all);
    rhs.AddEdge({1}, {4}, {1}, {1}).SetMutations(muts1 | std::views::all);
    rhs.AddEdge({2}, {5}, {2}, {0}).SetMutations(muts2 | std::views::all);
    rhs.AddEdge({3}, {5}, {3}, {1}).SetMutations(muts3 | std::views::all);
    rhs.AddEdge({4}, {6}, {4}, {0}).SetMutations(muts4 | std::views::all);
    rhs.AddEdge({5}, {6}, {5}, {1}).SetMutations(muts5 | std::views::all);
    rhs.BuildConnections();

    HistoryDAG merged = Merge({lhs, rhs});
    ToDOT(merged, LeafSet(merged), std::cout);
}

[[maybe_unused]]
static void test_two_merges() {
    HistoryDAG tree0;
    for (size_t i = 0; i < 5; ++i) tree0.AddNode({i});
    std::vector<Mutation> muts0{{{0}, 'A', 'A'}, {{1}, 'A', 'C'}};
    std::vector<Mutation> muts1{{{0}, 'A', 'A'}, {{1}, 'A', 'G'}};
    std::vector<Mutation> muts2{{{0}, 'A', 'A'}, {{1}, 'A', 'T'}};
    std::vector<Mutation> muts3{{{0}, 'A', 'A'}, {{1}, 'A', 'A'}};
    tree0.AddEdge({0}, {3}, {0}, {0}).SetMutations(muts0 | std::views::all);
    tree0.AddEdge({1}, {3}, {1}, {1}).SetMutations(muts1 | std::views::all);
    tree0.AddEdge({2}, {4}, {2}, {0}).SetMutations(muts2 | std::views::all);
    tree0.AddEdge({3}, {4}, {3}, {1}).SetMutations(muts3 | std::views::all);
    tree0.BuildConnections();

    HistoryDAG tree1;
    for (size_t i = 0; i < 5; ++i) tree1.AddNode({i});
    muts0 = {{{0}, 'A', 'A'}, {{1}, 'G', 'C'}};
    muts1 = {{{0}, 'A', 'A'}, {{1}, 'G', 'G'}};
    muts2 = {{{0}, 'A', 'A'}, {{1}, 'A', 'T'}};
    muts3 = {{{0}, 'A', 'A'}, {{1}, 'A', 'G'}};
    tree1.AddEdge({0}, {3}, {0}, {0}).SetMutations(muts0 | std::views::all);
    tree1.AddEdge({1}, {3}, {1}, {1}).SetMutations(muts1 | std::views::all);
    tree1.AddEdge({2}, {4}, {2}, {0}).SetMutations(muts2 | std::views::all);
    tree1.AddEdge({3}, {4}, {3}, {1}).SetMutations(muts3 | std::views::all);
    tree1.BuildConnections();

    HistoryDAG tree2;
    for (size_t i = 0; i < 5; ++i) tree2.AddNode({i});
    muts0 = {{{0}, 'A', 'A'}, {{1}, 'G', 'T'}};
    muts1 = {{{0}, 'A', 'A'}, {{1}, 'G', 'G'}};
    muts2 = {{{0}, 'A', 'A'}, {{1}, 'G', 'C'}};
    muts3 = {{{0}, 'A', 'A'}, {{1}, 'G', 'G'}};
    tree2.AddEdge({0}, {3}, {0}, {0}).SetMutations(muts0 | std::views::all);
    tree2.AddEdge({1}, {3}, {1}, {1}).SetMutations(muts1 | std::views::all);
    tree2.AddEdge({2}, {4}, {2}, {0}).SetMutations(muts2 | std::views::all);
    tree2.AddEdge({3}, {4}, {3}, {1}).SetMutations(muts3 | std::views::all);
    tree2.BuildConnections();

    // HistoryDAG merged0 = Merge({tree0});
    // ToDOT(merged0, LeafSet(merged0), std::cout);

    // HistoryDAG merged1 = Merge({tree1});
    // ToDOT(merged1, LeafSet(merged1), std::cout);

    // HistoryDAG merged2 = Merge({tree2});
    // ToDOT(merged2, LeafSet(merged2), std::cout);

    HistoryDAG merged = Merge({tree0, tree1, tree2});
    ToDOT(merged, LeafSet(merged), std::cout);
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
    HistoryDAG merged = Merge({reference, source});
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
    // std::cout << "\n";
	// test_simple();
    // test_two_merges();
    // test_real();
}

[[maybe_unused]] static const auto test_added = add_test({
	run_test, "Merge"});
