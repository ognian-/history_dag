#include "merge.hpp"

#include <iostream>
#include <valgrind/callgrind.h>

#include "test_common.hpp"
#include "history_dag_loader.hpp"
#include "benchmark.hpp"

void GetLabels(const HistoryDAG& tree, std::vector<NodeLabel>& labels,
    const std::string& refseq, const std::vector<CompactGenome>& mutations) {
    labels.resize(tree.GetNodes().size());

    for (Edge edge : tree.TraversePreOrder()) {
        const CompactGenome& muts = mutations.at(edge.GetId().value);
        NodeLabel& label = labels.at(edge.GetChild().GetId().value);
        const CompactGenome& parent_cgs = labels.at(edge.GetParent().GetId().value).first;
        label.first = parent_cgs;
        for (auto [pos, base] : muts) {
            if (base != refseq.at(pos - 1)) {
                label.first[pos] = base;
            }
        }
    }

    for (Node node : tree.TraversePostOrder()) {
        if (node.IsLeaf()) continue;
        LeafSet& leaf_set = labels.at(node.GetId().value).second;
        for (auto clade : node.GetClades()) {
            std::set<CompactGenome> clade_leafs;
            for (Node child : clade | std::views::transform(Transform::GetChild)) {
                if (child.IsLeaf()) {
                    clade_leafs.insert(labels.at(child.GetId().value).first);
                } else {
                    for (auto& child_leafs : labels.at(child.GetId().value).second) {
                        clade_leafs.insert(child_leafs.begin(), child_leafs.end());
                    }
                }
            }
            leaf_set.insert(clade_leafs);
        }
    }
}

[[maybe_unused]]
static void test_five_trees() {
    std::vector<std::vector<CompactGenome>> mutations;
    mutations.resize(5);

    HistoryDAG tree0 = LoadHistoryDAGFromProtobufGZ("data/test_5_trees/tree_0.pb.gz", mutations[0]);
    HistoryDAG tree1 = LoadHistoryDAGFromProtobufGZ("data/test_5_trees/tree_1.pb.gz", mutations[1]);
    HistoryDAG tree2 = LoadHistoryDAGFromProtobufGZ("data/test_5_trees/tree_2.pb.gz", mutations[2]);
    HistoryDAG tree3 = LoadHistoryDAGFromProtobufGZ("data/test_5_trees/tree_3.pb.gz", mutations[3]);
    HistoryDAG tree4 = LoadHistoryDAGFromProtobufGZ("data/test_5_trees/tree_4.pb.gz", mutations[4]);
    std::string refseq;
    HistoryDAG correct_result = LoadHistoryDAGFromJsonGZ("data/test_5_trees/full_dag.json.gz", refseq);
    
    std::vector<std::vector<NodeLabel>> labels;
    labels.resize(5);

    GetLabels(tree0, labels[0], refseq, mutations[0]);
    GetLabels(tree1, labels[1], refseq, mutations[1]);
    GetLabels(tree2, labels[2], refseq, mutations[2]);
    GetLabels(tree3, labels[3], refseq, mutations[3]);
    GetLabels(tree4, labels[4], refseq, mutations[4]);

    Merge merged(refseq, {tree0, tree1, tree2, tree3, tree4}, labels);
    merged.Run();

     assert_equal(correct_result.GetNodes().size(),
        merged.GetResult().GetNodes().size(),
        "Nodes count");
    
    assert_equal(correct_result.GetEdges().size(),
        merged.GetResult().GetEdges().size(),
        "Edges count");
}

[[maybe_unused]]
static void test_three_trees() {
    std::string refseq0, refseq1, refseq2, refseq_correct;

    HistoryDAG correct_result =
        LoadHistoryDAGFromJsonGZ("data/test_3_trees/littledag.json.gz", refseq_correct);
    HistoryDAG tree0 =
        LoadHistoryDAGFromJsonGZ("data/test_3_trees/final-tree-1.nh1.pb.json.gz", refseq0);
    HistoryDAG tree1 =
        LoadHistoryDAGFromJsonGZ("data/test_3_trees/final-tree-2.nh1.pb.json.gz", refseq1);
    HistoryDAG tree2 =
        LoadHistoryDAGFromJsonGZ("data/test_3_trees/final-tree-3.nh1.pb.json.gz", refseq2);
    
    assert_equal(refseq0, refseq1, "Reference sequence");
    assert_equal(refseq1, refseq2, "Reference sequence");
    assert_equal(refseq2, refseq_correct, "Reference sequence");

    std::vector<std::vector<NodeLabel>> labels;
    labels.resize(3);

    labels[0] = LoadLabelsJsonGZ("data/test_3_trees/final-tree-1.nh1.pb.json.gz");
    labels[1] = LoadLabelsJsonGZ("data/test_3_trees/final-tree-2.nh1.pb.json.gz");
    labels[2] = LoadLabelsJsonGZ("data/test_3_trees/final-tree-3.nh1.pb.json.gz");

    Merge merged(refseq0, {tree0, tree1, tree2}, labels);
    merged.Run();

    assert_equal(correct_result.GetNodes().size(),
        merged.GetResult().GetNodes().size(),
        "Nodes count");
    
    assert_equal(correct_result.GetEdges().size(),
        merged.GetResult().GetEdges().size(),
        "Edges count");
}

[[maybe_unused]] static const auto test0_added = add_test({
	test_three_trees, "Merge 3 trees"});

[[maybe_unused]] static const auto test1_added = add_test({
	test_five_trees, "Merge 5 trees"});
