/*
  Collection is a lightweight object with the properties of a sized container
  with forward iterators. It is meant to be passed by value, as it merely holds
  a reference to the actual storage. It is also capable of lazily applying a
  transform to each element, which is for example useful when representing a
  storage of NodeIs, but returns Node instances to it's users.
*/
#pragma once

#include <utility>
#include <cstddef>
#include <iterator>

template <typename Storage, typename Transform>
class Collection {
public:

	Collection(Storage& data, Transform&& transform);

	using Iter = decltype(std::declval<Storage>().begin());

	class Iterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using value_type = std::decay_t<decltype(*std::declval<Iter>())>;
		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = const pointer;
		using const_reference = const reference;
	
		Iterator(Iter iter, const Collection& collection);
		Iterator(const Iterator&) = default;
		Iterator& operator=(const Iterator& other);
		auto operator*() const;
		Iterator& operator++();
		Iterator operator++(int);
		bool operator==(const Iterator& other) const;
		bool operator!=(const Iterator& other) const;
		
	private:
		Iter iter_;
		Collection collection_;
	};
	
	Iterator begin() const;
	Iterator end() const;
	size_t size() const;
	bool empty() const;
	
private:
	friend class Iterator;
	Storage& data_;
	Transform transform_;
};

///////////////////////////////////////////////////////////////////////////////

template <typename Storage, typename Transform>
Collection<Storage, Transform>::Collection(Storage& data,
	Transform&& transform) : data_{data}, transform_{transform} {}

template <typename Storage, typename Transform>
Collection<Storage, Transform>::Iterator::Iterator(Iter iter,
	const Collection& collection) : iter_{iter}, collection_{collection} {}

template <typename Storage, typename Transform>
typename Collection<Storage, Transform>::Iterator&
Collection<Storage, Transform>::Iterator::operator=(const Iterator& other) {
	iter_ = other.iter_;
	return *this;
}

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
Collection<Storage, Transform>::Iterator::operator++(int) {
	return {iter_++, collection_};
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
Collection<Storage, Transform>::begin() const {
	return {data_.begin(), *this};
}

template <typename Storage, typename Transform>
typename Collection<Storage, Transform>::Iterator
Collection<Storage, Transform>::end() const {
	return {data_.end(), *this};
}

template <typename Storage, typename Transform>
size_t Collection<Storage, Transform>::size() const {
	return data_.size();
}

template <typename Storage, typename Transform>
bool Collection<Storage, Transform>::empty() const {
	return data_.empty();
}
