#pragma once

#include <utility>
#include <cstddef>
#include <iterator>
#include <memory>
#include <cassert>

template <typename Target>
class FlatCollection {
public:
	
	explicit FlatCollection(Target&& target);
	
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
		auto operator*() const;
		Iterator& operator++();
		Iterator operator++(int);
		bool operator==(const Iterator& other) const;
		bool operator!=(const Iterator& other) const;
	private:
		OuterIter outer_iter_;
		std::unique_ptr<InnerIter> inner_iter_; //TODO make iter assignable
		FlatCollection& collection_;
	};
	
	Iterator begin();
	Iterator end();
	size_t size() const;
	bool empty() const;
	
private:
	Target target_;
	size_t size_;
	std::unique_ptr<OuterIter> outer_last_; //TODO make iter assignable
};

///////////////////////////////////////////////////////////////////////////////

template <typename Target>
FlatCollection<Target>::FlatCollection(Target&& target) : target_{target},
	size_{0}, outer_last_{std::make_unique<OuterIter>(target_.begin())} {
	OuterIter outer = target_.begin();
	for (size_t i = 0; i < target_.size(); ++i) {
		size_ += (*outer).size();
		if (i + 1 == target_.size()) outer_last_ =
			std::make_unique<OuterIter>(outer);
		++outer;
	}
}

template <typename Target>
FlatCollection<Target>::Iterator::Iterator(OuterIter outer_iter,
	InnerIter inner_iter, FlatCollection& collection) :
		outer_iter_{outer_iter},
		inner_iter_{std::make_unique<InnerIter>(inner_iter)},
		collection_{collection} {}

template <typename Target>
auto FlatCollection<Target>::Iterator::operator*() const {
	return *(inner_iter_.get());
}

template <typename Target>
typename FlatCollection<Target>::Iterator&
FlatCollection<Target>::Iterator::operator++() {
	//TODO
	if (++(*inner_iter_) == (*outer_iter_).end()) {
		OuterIter current = outer_iter_++;
		if (outer_iter_ == collection_.target_.end()) {
			inner_iter_ = std::make_unique<InnerIter>((*current).end());
		} else {
			inner_iter_ = std::make_unique<InnerIter>((*outer_iter_).begin());
		}
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
	return outer_iter_ == other.outer_iter_ &&
		inner_iter_ == other.inner_iter_;
}

template <typename Target>
bool FlatCollection<Target>::Iterator::operator!=(
	const FlatCollection<Target>::Iterator& other) const {
	return outer_iter_ != other.outer_iter_ ||
		inner_iter_ != other.inner_iter_;
}

template <typename Target>
typename FlatCollection<Target>::Iterator FlatCollection<Target>::begin() {
	assert(!empty()); //TODO
	return {target_.begin(), (*target_.begin()).begin(), *this};
}

template <typename Target>
typename FlatCollection<Target>::Iterator FlatCollection<Target>::end() {
	assert(!empty()); //TODO
	auto outer = target_.begin();
	std::advance(outer, target_.size() - 1);
	return {outer, (*outer).end(), *this};
}

template <typename Target>
size_t FlatCollection<Target>::size() const {
	return size_;
}

template <typename Target>
bool FlatCollection<Target>::empty() const {
	return size_ > 0;
}
