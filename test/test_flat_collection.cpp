#include <vector>
#include <type_traits>

#include "test_common.hpp"

#include "flat_collection.hpp"
#include "collection.hpp"

static void test_lvalue_vec_vec() {
    std::vector<std::vector<int>> data = {
        {1,2,3},
        {4,5,6},
        {7,8,9}
    };

	FlatCollection x{data};

    static_assert(std::is_same_v<typename decltype(x)::TargetType,
        std::vector<std::vector<int>>&>,
        "lvalue_vec_vec: wrong ref type");

    assert_equal(x.size(), size_t(9), "lvalue_vec_vec: wrong size");

    int check = 1;
    for (int i : x) {
        assert_equal(i, check++, "lvalue_vec_vec: wrong element");
    }
}

static void test_rvalue_vec_vec() {
	FlatCollection x{
        std::vector<std::vector<int>>{
            {1,2,3},
            {4,5,6},
            {7,8,9}
        }
    };

    static_assert(std::is_same_v<typename decltype(x)::TargetType,
        std::vector<std::vector<int>>>,
        "rvalue_vec_vec: wrong ref type");

    assert_equal(x.size(), size_t(9), "rvalue_vec_vec: wrong size");

    int check = 1;
    for (int i : x) {
        assert_equal(i, check++, "rvalue_vec_vec: wrong element");
    }
}

static void test_empty() {
    FlatCollection x1{
        std::vector<std::vector<int>>{}
    };
    assert_true(x1.empty(), "empty");
    for ([[maybe_unused]] auto i : x1) assert_true(false, "empty");

    FlatCollection x2{
        std::vector<std::vector<int>>{
            {},{},{}
        }
    };
    assert_true(x2.empty(), "empty");
    for ([[maybe_unused]] auto i : x2) assert_true(false, "empty");

    FlatCollection x3{
        std::vector<std::vector<int>>{
            {},{1},{}
        }
    };
    assert_false(x3.empty(), "empty");
    assert_equal(x3.size(), size_t(1), "empty");
    for (auto i : x3) assert_equal(i, 1, "empty");

    auto v4 = std::vector<std::vector<int>>{
            {},{},{1},{},{}
        };

    auto c4 = Collection{v4,
		[](std::vector<int>& j, size_t) {
		return Collection{j, [](int k, size_t) {
			return k;
		}};
	}};

    FlatCollection x4{c4};
    assert_false(x4.empty(), "empty");
    assert_equal(x4.size(), size_t(1), "empty");
    for (auto i : x4) assert_equal(i, 1, "empty");
}

static void run_test() {
	test_lvalue_vec_vec();
    test_rvalue_vec_vec();
    test_empty();
}

[[maybe_unused]] static const auto test_added = add_test({
	run_test, "FlatCollection"});
