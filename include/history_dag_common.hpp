#pragma once

#include <cstddef>
#include <limits>
#include <vector>

static constexpr size_t NoId = std::numeric_limits<size_t>::max();

struct NodeId {
	size_t value = NoId;
};

struct EdgeId {
	size_t value = NoId;
};

inline bool operator==(NodeId lhs, NodeId rhs) {
	return lhs.value == rhs.value;
}

template <typename T, typename Id>
static T& GetOrInsert(std::vector<T>& data, Id id) {
	if constexpr (std::is_same_v<Id, size_t>) {
		if (id >= data.size()) data.resize(id + 1);
		return data[id];
	} else {
		if (id.value >= data.size()) data.resize(id.value + 1);
		return data[id.value];
	}
}
