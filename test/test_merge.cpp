#include <iostream>

#include "test_common.hpp"

#include "merge_dags.hpp"
#include "hdag_ops.hpp"

static void test_merge() {
    HistoryDAG dag1;

    dag1.AddNode({0}, std::string{"AA"});
    dag1.AddNode({1}, std::string{"CT"});
    dag1.AddNode({2}, std::string{"TA"});
    dag1.AddNode({3}, std::string{"CC"});
    dag1.AddNode({4}, std::string{"CG"});
    dag1.AddNode({5}, std::string{"CA"});
    dag1.AddNode({6}, std::string{"CA"});

    dag1.AddEdge({0}, dag1.GetNode({4}), dag1.GetNode({0}), 0);
    dag1.AddEdge({1}, dag1.GetNode({4}), dag1.GetNode({1}), 0);
    dag1.AddEdge({2}, dag1.GetNode({5}), dag1.GetNode({2}), 0);
    dag1.AddEdge({3}, dag1.GetNode({5}), dag1.GetNode({3}), 0);
    dag1.AddEdge({4}, dag1.GetNode({6}), dag1.GetNode({4}), 0);
    dag1.AddEdge({5}, dag1.GetNode({6}), dag1.GetNode({5}), 0);

    HistoryDAG dag2;

    dag2.AddNode({0}, std::string{"AA"});
    dag2.AddNode({1}, std::string{"CT"});
    dag2.AddNode({2}, std::string{"TA"});
    dag2.AddNode({3}, std::string{"CC"});
    dag2.AddNode({4}, std::string{"CA"});
    dag2.AddNode({5}, std::string{"CG"});
    dag2.AddNode({6}, std::string{"CA"});

    dag2.AddEdge({0}, dag2.GetNode({4}), dag2.GetNode({0}), 0);
    dag2.AddEdge({1}, dag2.GetNode({4}), dag2.GetNode({1}), 0);
    dag2.AddEdge({2}, dag2.GetNode({5}), dag2.GetNode({2}), 0);
    dag2.AddEdge({3}, dag2.GetNode({5}), dag2.GetNode({3}), 0);
    dag2.AddEdge({4}, dag2.GetNode({6}), dag2.GetNode({4}), 0);
    dag2.AddEdge({5}, dag2.GetNode({6}), dag2.GetNode({5}), 0);

    MergeDAGs merge;

    auto result = merge(dag1, dag2);

    ToDOT(result, std::cout);
}

static void run_test() {
	test_merge();
}

[[maybe_unused]] static const auto test_added = add_test({
	run_test, "Merge DAGs"});
