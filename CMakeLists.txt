cmake_minimum_required(VERSION 3.11)
project (larch)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz)
FetchContent_MakeAvailable(json)

find_package(Protobuf REQUIRED)

protobuf_generate_cpp(DAG_PROTO_SRCS DAG_PROTO_HDRS proto/dag.proto)

add_executable(larch
    ${DAG_PROTO_SRCS} ${DAG_PROTO_HDRS}
    src/history_dag.cpp
    src/history_dag_loader.cpp
    src/history_dag_node_storage.cpp
    src/zlib_stream.cpp
    test/main.cpp
    test/test_merge.cpp)

target_compile_options(larch PUBLIC -std=c++20 -Werror -Wall -Wextra -pedantic -pedantic-errors -Wold-style-cast)
target_include_directories(larch PUBLIC include test ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(larch PUBLIC -lz)
target_link_libraries(larch PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(larch PUBLIC ${Protobuf_LIBRARIES})
