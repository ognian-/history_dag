#include <fstream>
#include <vector>
#include <unordered_map>

#include "history_dag_loader.hpp"
#include "zlib_stream.hpp"
#include "proto/parsimony.pb.h"
#include "newick.hpp"

[[nodiscard]] HistoryDAG LoadHistoryDAG(const std::string& path) {
    std::ifstream in_compressed(path.c_str());
    assert(in_compressed);
    zlib::ZStringBuf zbuf(in_compressed, 1, 128 * 1024 * 1024);
    std::istream in(&zbuf);
    Parsimony::data data;
    data.ParseFromIstream(&in);
    HistoryDAG dag;
    size_t edge_id = 0;
    ParseNewick(data.newick(), [&](size_t id, std::string label,
        std::optional<double> branch_length) {
            dag.AddNode({id}).SetLabel(label);
            std::ignore = branch_length;
        }, [&](size_t parent, size_t child) {
            dag.AddEdge({edge_id++}, dag.GetNode({parent}),
                dag.GetNode({child}), 0);
        });
    dag.BuildConnections();

    size_t muts_idx = 0;
    for (MutableNode node : dag.TraversePreOrder()) {
        const auto& muts = data.node_mutations()[muts_idx++];
        if (node.IsRoot()) continue;
        node.GetSingleParent().SetMutations(muts.mutation() |
            std::views::transform([](auto& mut) -> Mutation {
                return {
                    static_cast<size_t>(mut.position()),
                    static_cast<char>(mut.ref_nuc()),
                    static_cast<char>(mut.par_nuc()),
                    mut.mut_nuc()
                };
            }));
    }

    std::unordered_map<std::string, std::vector<std::string>> condensed_nodes;
    for (auto& i : data.condensed_nodes()) {
        condensed_nodes[i.node_name()] = {i.condensed_leaves().begin(),
            i.condensed_leaves().end()};
    }

    std::vector<std::vector<std::string>> metadata;
    for (auto& i : data.metadata()) {
        metadata.push_back({i.clade_annotations().begin(),
            i.clade_annotations().end()});
    }

    return dag;
}
