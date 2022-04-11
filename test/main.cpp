#include <iostream>

#include "test_common.hpp"

static std::vector<Test>& get_all_tests() {
	static std::vector<Test> all_tests{};
	return all_tests;
}

bool add_test(const Test& test) noexcept {
	get_all_tests().push_back(test);
	return true;
}

int main(int /*argc*/, char* /*argv*/ []) {

	size_t failed = 0, ran = 0;
	const auto num_tests = get_all_tests().size();
	std::cout << "Running " << num_tests << " tests" << std::endl;
	for (auto&& test : get_all_tests()) {
		++ran;
		std::cout << "Running test: " << test.name <<
			" (" << ran << "/" << num_tests << ") ...";
		try {
			test.entry();
			std::cout << " passed." << std::endl;
		} catch (const std::exception& e) {
			++failed;
			std::cerr << std::endl << "Test '" << test.name <<
				"' failed with '" << e.what() << "'" << std::endl;
		}
	}
	if (failed) {
		std::cerr << "Failed tests: " << failed << "/" <<
			get_all_tests().size() << std::endl;
		return EXIT_FAILURE;
	} else {
		std::cout << "All tests passed" << std::endl;
		return EXIT_SUCCESS;
	}
}
