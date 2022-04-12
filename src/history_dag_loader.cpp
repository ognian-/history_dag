#include <fstream>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "history_dag_loader.hpp"
#include "zlib_stream.hpp"
#include "proto/parsimony.pb.h"
#include "newick.hpp"

HistoryDAG LoadHistoryDAG(const std::string& path) {
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
    ParseNewick(data.newick(), [&](size_t id, std::string label) {
        dag.AddNode({id}, label);
    }, [&](size_t parent, size_t child) {
        dag.AddEdge({edge_id++}, dag.GetNode({parent}),
            dag.GetNode({child}), 0);
    });
    dag.Finalize();
    return dag;
}
