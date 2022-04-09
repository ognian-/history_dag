#include <type_traits>
#include <limits>
#include <iterator>
#include <algorithm>
#include <cassert>
#include <vector>

#pragma once

static constexpr size_t NoId = std::numeric_limits<size_t>::max();

struct NodeId {
	size_t value = NoId;
};

struct EdgeId {
	size_t value = NoId;
};

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

template <typename Storage, typename Transform>
class Collection {
public:

	Collection(Storage& data, Transform&& transform);
	
	using Iter = decltype(std::declval<Storage>().begin());

	class Iterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = std::decay_t<decltype(*std::declval<Iter>())>;
		using pointer = value_type*;
		using reference = value_type&;
	
		Iterator(Iter iter, Collection& collection);
		auto operator*() const;
		Iterator& operator++();
		Iterator operator++(int) const;
		bool operator==(const Iterator& other) const;
		bool operator!=(const Iterator& other) const;
	private:
		Iter iter_;
		Collection& collection_;
	};
	
	Iterator begin();
	Iterator end();
	size_t size();
	bool empty();
	
private:
	friend class Iterator;
	Storage& data_;
	Transform transform_;
};

class HistoryDAG;
class NodeStorage;
class Edge;

class Node {
public:
	Node(HistoryDAG& dag, NodeId id);
	NodeId GetId() const;
	inline auto GetSequence() const;
	inline auto GetParents() const;
	inline auto GetClades() const;
	inline auto GetLeafsBelow() const;
private:
	NodeStorage& GetStorage() const;
	HistoryDAG& dag_;
	NodeId id_;
};

class EdgeStorage;

class Edge {
public:
	Edge(HistoryDAG& dag, EdgeId id);
	EdgeId GetId() const;
	Node GetParent() const;
	Node GetChild() const;
	size_t GetClade() const;
private:
	EdgeStorage& GetStorage() const;
	HistoryDAG& dag_;
	EdgeId id_;
};

class NodeStorage {
public:
	
	void AddEdge(Edge edge, bool is_parent);

	std::vector<char> sequence_;
	std::vector<EdgeId> parents_;
	std::vector<std::vector<EdgeId>> clades_;
	std::vector<NodeId> leafs_below_;
};

class EdgeStorage {
public:
	NodeId parent_;
	NodeId child_;
	size_t clade_;
};

class HistoryDAG {
public:
	
	template <typename Sequence>
	Node AddNode(NodeId id, Sequence&& sequence);
	
	Edge AddEdge(EdgeId id, Node parent, Node child, size_t clade);
	
	inline auto GetNodes();
	inline auto GetEdges();
	
	Node GetNode(NodeId id);
	Edge GetEdge(EdgeId id);
	
private:
	friend class Node;
	friend class Edge;
	
	std::vector<NodeStorage> nodes_;
	std::vector<EdgeStorage> edges_;
};

///////////////////////////////////////////////////////////////////////////////

template <typename Storage, typename Transform>
Collection<Storage, Transform>::Collection(Storage& data,
	Transform&& transform) : data_{data}, transform_{transform} {}

template <typename Storage, typename Transform>
Collection<Storage, Transform>::Iterator::Iterator(Iter iter,
	Collection& collection) : iter_{iter}, collection_{collection} {}

template <typename Storage, typename Transform>
auto Collection<Storage, Transform>::Iterator::operator*() const {
	return collection_.transform_(*iter_, iter_ - collection_.data_.begin());
}

template <typename Storage, typename Transform>
typename Collection<Storage, Transform>::Iterator&
Collection<Storage, Transform>::Iterator::operator++() {
	++iter_;
	return *this;
}

template <typename Storage, typename Transform>
typename Collection<Storage, Transform>::Iterator
Collection<Storage, Transform>::Iterator::operator++(int) const {
	return {iter_++};
}

template <typename Storage, typename Transform>
bool Collection<Storage, Transform>::Iterator::operator==(
	const Iterator& other) const {
	return iter_ == other.iter_;
}

template <typename Storage, typename Transform>
bool Collection<Storage, Transform>::Iterator::operator!=(
	const Iterator& other) const {
	return iter_ != other.iter_;
}

template <typename Storage, typename Transform>
typename Collection<Storage, Transform>::Iterator
Collection<Storage, Transform>::begin() {
	return {data_.begin(), *this};
}

template <typename Storage, typename Transform>
typename Collection<Storage, Transform>::Iterator
Collection<Storage, Transform>::end() {
	return {data_.end(), *this};
}

template <typename Storage, typename Transform>
size_t Collection<Storage, Transform>::size() {
	return data_.size();
}

template <typename Storage, typename Transform>
bool Collection<Storage, Transform>::empty() {
	return data_.empty();
}

///////////////////////////////////////////////////////////////////////////////

auto Node::GetSequence() const {
	return Collection{GetStorage().sequence_, [](char x, size_t) {
		return x;
	}};
}

auto Node::GetParents() const {
	return Collection{GetStorage().parents_, [this](EdgeId idx, size_t) {
		return Edge{dag_, idx};
	}};
}

auto Node::GetClades() const {
	return Collection{GetStorage().clades_,
		[this](std::vector<EdgeId>& clade, size_t) {
		return Collection{clade, [this](EdgeId idx, size_t) {
			return Edge{dag_, idx};
		}};
	}};
}

auto Node::GetLeafsBelow() const {
	return Collection{GetStorage().leafs_below_, [this](NodeId idx, size_t) {
		return Node{dag_, idx};
	}};
}

///////////////////////////////////////////////////////////////////////////////

template <typename Sequence>
Node HistoryDAG::AddNode(NodeId id, Sequence&& sequence) {
	assert(id.value != NoId);
	auto& storage = GetOrInsert(nodes_, id);
	storage.sequence_.clear();
	std::copy(sequence.begin(), sequence.end(),
		std::back_inserter(storage.sequence_));
	return {*this, id};
}

auto HistoryDAG::GetNodes() {
	return Collection{nodes_, [this](NodeStorage&, size_t idx) {
		return Node{*this, {idx}};
	}};
}

auto HistoryDAG::GetEdges() {
	return Collection{edges_, [this](EdgeStorage&, size_t idx) {
		return Edge{*this, {idx}};
	}};
}

///////////////////////////////////////////////////////////////////////////////
