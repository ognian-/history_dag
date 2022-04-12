#include <fstream>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "history_dag_loader.hpp"
#include "zlib_stream.hpp"
#include "proto/parsimony.pb.h"

HistoryDAG LoadHistoryDAG(const std::string& path) {
    Parsimony::data data;
    std::ifstream in_compressed(path.c_str());
    assert(in_compressed);
    zlib::ZStringBuf zbuf(in_compressed, 1024, 2048);
    std::istream in(&zbuf);
    google::protobuf::io::IstreamInputStream stream(&in);
    google::protobuf::io::CodedInputStream input(&stream);
    data.ParseFromCodedStream(&input);
    HistoryDAG dag;
    return dag;
}
