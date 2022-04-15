#include <fstream>
#include <vector>
#include <unordered_map>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "history_dag_loader.hpp"
#include "zlib_stream.hpp"
#include "proto/parsimony.pb.h"
#include "newick.hpp"

class NodeMutationsStorage {
public:
    NodeMutationsStorage() = default;
    template <typename T>
    NodeMutationsStorage(size_t position, char reference_nucleotide,
        char parent_nucleotide, const T& mutated_nucleotide) :
        position_{position},
        reference_nucleotide_{reference_nucleotide},
        parent_nucleotide_{parent_nucleotide},
        mutated_nucleotide_{std::begin(mutated_nucleotide),
            std::end(mutated_nucleotide)} {}

    size_t position_ = NoId;
    char reference_nucleotide_ = 0;
    char parent_nucleotide_ = 0;
    std::vector<char> mutated_nucleotide_ = {};
};

[[nodiscard]] HistoryDAG LoadHistoryDAG(const std::string& path) {
    std::ifstream in_compressed(path.c_str());
    assert(in_compressed);
    zlib::ZStringBuf zbuf(in_compressed, 8 * 1024, 16 * 1024);
    std::istream in(&zbuf);
    google::protobuf::io::IstreamInputStream stream(&in);
    google::protobuf::io::CodedInputStream input(&stream);
    Parsimony::data data;
    data.ParseFromCodedStream(&input);
    HistoryDAG dag;
    size_t edge_id = 0;
    std::unordered_map<std::string, size_t> node_ids;
    ParseNewick(data.newick(), [&](size_t id, std::string label,
        std::optional<double> branch_length) {
            node_ids[label] = id;
            dag.AddNode({id}, std::ranges::empty_view<char>{});
            std::ignore = branch_length;
        }, [&](size_t parent, size_t child) {
            dag.AddEdge({edge_id++}, dag.GetNode({parent}),
                dag.GetNode({child}), 0);
        });

    std::vector<std::vector<NodeMutationsStorage>> node_mutations;
    std::transform(data.node_mutations().begin(), data.node_mutations().end(),
		std::back_inserter(node_mutations),
        [](decltype(*data.node_mutations().begin()) i) {
            std::vector<NodeMutationsStorage> result;
            std::transform(i.mutation().begin(), i.mutation().end(),
            std::back_inserter(result),
            [](decltype(*i.mutation().begin()) j) {
                return NodeMutationsStorage{
                    static_cast<size_t>(j.position()),
                    static_cast<char>(j.ref_nuc()),
                    static_cast<char>(j.par_nuc()),
                    j.mut_nuc()
                };
            });
            return result;
        });

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

    dag.Finalize();
    return dag;
}
