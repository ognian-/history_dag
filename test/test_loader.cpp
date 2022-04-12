#include <iostream>

#include "test_common.hpp"

#include "history_dag_loader.hpp"
#include "hdag_ops.hpp"



static void test_loading() {
    HistoryDAG dag = LoadHistoryDAG("public-latest.all.masked.pb.gz");
    ToDOT(dag, std::cout);
}

static void run_test() {
	test_loading();
}

[[maybe_unused]] static const auto test_added = add_test({
	run_test, "Loading hDAG"});
