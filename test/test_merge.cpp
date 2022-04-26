#include <iostream>

#include "test_common.hpp"

#include "history_dag_loader.hpp"
#include "merge.hpp"
#include "hdag_ops.hpp"
#include "benchmark.hpp"

static std::vector<std::set<NodeId>> GetCladeSets(const HistoryDAG& dag) {
    std::vector<std::set<NodeId>> clade_sets;
    for (Node i : dag.TraversePostOrder()) {
        for (auto child : i.GetChildren() |
            std::views::transform(Transform::GetChild)) {
            auto& cs = GetOrInsert(clade_sets, i.GetId());
            if (child.IsLeaf()) {
                cs.insert(child.GetId());
            } else {
                auto& leafs = clade_sets.at(child.GetId().value);
                cs.insert(std::begin(leafs), std::end(leafs));
            }
        }
    }
    return clade_sets;
}

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
    lhs.AddEdge({1}, {4}, {1}, {0}).SetMutations(muts1 | std::views::all);
    lhs.AddEdge({2}, {5}, {2}, {0}).SetMutations(muts2 | std::views::all);
    lhs.AddEdge({3}, {5}, {3}, {0}).SetMutations(muts3 | std::views::all);
    lhs.AddEdge({4}, {6}, {4}, {0}).SetMutations(muts4 | std::views::all);
    lhs.AddEdge({5}, {6}, {5}, {0}).SetMutations(muts5 | std::views::all);
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
    rhs.AddEdge({1}, {4}, {1}, {0}).SetMutations(muts1 | std::views::all);
    rhs.AddEdge({2}, {5}, {2}, {0}).SetMutations(muts2 | std::views::all);
    rhs.AddEdge({3}, {5}, {3}, {0}).SetMutations(muts3 | std::views::all);
    rhs.AddEdge({4}, {6}, {4}, {0}).SetMutations(muts4 | std::views::all);
    rhs.AddEdge({5}, {6}, {5}, {0}).SetMutations(muts5 | std::views::all);
    rhs.BuildConnections();

    ToDOT(lhs, GetCladeSets(lhs), std::cout);
    ToDOT(rhs, GetCladeSets(rhs), std::cout);

    HistoryDAG merged = Merge(lhs, rhs);

    ToDOT(merged, GetCladeSets(merged), std::cout);
}

static void test_real() {
    HistoryDAG correct_result =
        LoadHistoryDAGFromJsonGZ("data/merge_result.json.gz");
    HistoryDAG source =
        LoadHistoryDAGFromProtobufGZ("data/1final-tree-1.nh4.pb.gz");
    HistoryDAG reference =
        LoadHistoryDAGFromProtobufGZ("data/1final-tree-1.nh1.pb.gz");
    Benchmark merge_time;
    merge_time.start();
    HistoryDAG merged = Merge(reference, source);
    merge_time.stop();
    std::cout << "\nDAGs merged in " << merge_time.durationMs() << " ms\n";

    std::cout << "Source nodes: " << source.GetNodes().size() << "\n";
    std::cout << "Source edges: " << source.GetEdges().size() << "\n\n";

    std::cout << "Reference nodes: " << reference.GetNodes().size() << "\n";
    std::cout << "Reference edges: " << reference.GetEdges().size() << "\n\n";

    std::cout << "Correct nodes: " << correct_result.GetNodes().size() << "\n";
    std::cout << "Correct edges: " << correct_result.GetEdges().size() << "\n";

    std::cout << "Merged nodes: " << merged.GetNodes().size() << "\n";
    std::cout << "Merged edges: " << merged.GetEdges().size() << "\n";  
}

static void run_test() {
	test_simple();
    test_real();
}

[[maybe_unused]] static const auto test_added = add_test({
	run_test, "Merge"});
