#include <fstream>
#include <vector>
#include <unordered_map>

#include "nlohmann/json.hpp"

#include "history_dag_loader.hpp"
#include "zlib_stream.hpp"
#include "proto/parsimony.pb.h"
#include "newick.hpp"

HistoryDAG LoadHistoryDAGFromProtobufGZ(const std::string& path) {
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

HistoryDAG LoadHistoryDAGFromJsonGZ(const std::string& path) {
    std::ifstream in_compressed(path);
    assert(in_compressed);
    zlib::ZStringBuf zbuf(in_compressed, 1, 128 * 1024 * 1024);
    std::istream in(&zbuf);
    nlohmann::json json;
    in >> json;
    HistoryDAG result;

    size_t id = 0;
    for ([[maybe_unused]] auto& i : json["nodes"]) result.AddNode({id++});
    id = 0;
    for (auto& i : json["edges"]) {
        MutableEdge edge = result.AddEdge({id++}, {i[0]}, {i[1]}, i[2]);
        size_t child_id = i[1];
        size_t cg_idx = json["nodes"][child_id][0];
        std::vector<Mutation> cg;
        for (auto& j : json["compact_genomes"][cg_idx]) {
            cg.push_back({
                static_cast<size_t>(j[0]),
                j[1][1].get<std::string>()[0],
                j[1][0].get<std::string>()[0],
                std::views::empty<char>
            });
        }
        edge.SetMutations(std::views::all(cg));
    }
    result.BuildConnections();

    return result;
}
