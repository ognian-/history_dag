/*
  FlatCollection is representing a two-dimensional container as a linear
  sequence of elements. If constructed with a lvalue reference it behaves as a
  lightweight view into the actual storage. If constructed with a rvalue
  reference it will assume ownership and move the data into itself.
*/
#pragma once

#include <utility>
#include <cstddef>
#include <iterator>
#include <cassert>
#include <type_traits>
#include <optional>

template <typename Target>
class FlatCollection {
public:
	
	template <typename T>
	constexpr explicit FlatCollection(T&& target);
	
	using TargetType = Target;
	using OuterIter = decltype(std::declval<Target>().begin());
	using InnerCollection = decltype(*std::declval<OuterIter>());
	using InnerIter = decltype(std::declval<InnerCollection>().begin());

	class Iterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using value_type = std::decay_t<decltype(*std::declval<InnerIter>())>;
		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = const pointer;
		using const_reference = const reference;
	
		Iterator(OuterIter outer_iter, InnerIter inner_iter,
			FlatCollection& collection);
		Iterator(OuterIter outer_iter, FlatCollection& collection);
		Iterator(const Iterator&) = default;
		auto operator*() const;
		Iterator& operator++();
		Iterator operator++(int);
		bool operator==(const Iterator& other) const;
		bool operator!=(const Iterator& other) const;
	private:
		OuterIter outer_iter_;
		std::optional<InnerIter> inner_iter_;
		FlatCollection& collection_;
	};
	
	Iterator begin();
	Iterator end();
	size_t size() const;
	bool empty() const;
	
private:
	Target target_;
	const size_t size_;
};

template <typename T>
FlatCollection(T&) -> FlatCollection<T&>;

template <typename T>
FlatCollection(T&&) -> FlatCollection<T>;

///////////////////////////////////////////////////////////////////////////////

template <typename Target>
template <typename T>
constexpr FlatCollection<Target>::FlatCollection(T&& target) : target_{target},
	size_{[this](){
		size_t size = 0;
		for (auto i : target_) size += i.size();
		return size;
	}()} {}

template <typename Target>
FlatCollection<Target>::Iterator::Iterator(OuterIter outer_iter,
	InnerIter inner_iter, FlatCollection& collection) :
		outer_iter_{outer_iter},
		inner_iter_{inner_iter},
		collection_{collection} {}

template <typename Target>
FlatCollection<Target>::Iterator::Iterator(OuterIter outer_iter,
	FlatCollection& collection) :
		outer_iter_{outer_iter},
		inner_iter_{std::nullopt},
		collection_{collection} {}

template <typename Target>
auto FlatCollection<Target>::Iterator::operator*() const {
	assert(inner_iter_.has_value());
	return **inner_iter_;
}

template <typename Target>
typename FlatCollection<Target>::Iterator&
FlatCollection<Target>::Iterator::operator++() {
	assert(inner_iter_.has_value());
	if (++*inner_iter_ == (*outer_iter_).end()) {
		while (++outer_iter_ != collection_.target_.end()) {
			if (not (*outer_iter_).empty()) {
				*inner_iter_ = (*outer_iter_).begin();
				return *this;
			}
		}
		inner_iter_ = std::nullopt;
	}
	return *this;
}

template <typename Target>
typename FlatCollection<Target>::Iterator::Iterator
FlatCollection<Target>::Iterator::operator++(int) {
	Iterator result = *this;
	++*this;
	return result;
}

template <typename Target>
bool FlatCollection<Target>::Iterator::operator==(
	const FlatCollection<Target>::Iterator& other) const {
	if (outer_iter_ == collection_.target_.end() &&
		other.outer_iter_ == collection_.target_.end()) return true;
	if (outer_iter_ != other.outer_iter_) return false;
	return inner_iter_ == other.inner_iter_;
}

template <typename Target>
bool FlatCollection<Target>::Iterator::operator!=(
	const FlatCollection<Target>::Iterator& other) const {
	return not (*this == other);
}

template <typename Target>
typename FlatCollection<Target>::Iterator FlatCollection<Target>::begin() {
	for (auto i = target_.begin(); i != target_.end(); ++i) {
		if (not (*i).empty()) return {i, (*i).begin(), *this};
	}
	return end();
}

template <typename Target>
typename FlatCollection<Target>::Iterator FlatCollection<Target>::end() {
	return {target_.end(), *this};
}

template <typename Target>
size_t FlatCollection<Target>::size() const {
	return size_;
}

template <typename Target>
bool FlatCollection<Target>::empty() const {
	return size_ == 0;
}
