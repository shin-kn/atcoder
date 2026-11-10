#pragma once
#include "balanced_tree.cpp"
#include "base.cpp"
#include "light_array.cpp"
#include <compare>

// valuetree's type is named only when the values can be ordered, so that
// AVLTree<Pair<U, ull>, ull> is never formed for an unordered U (e.g. Mod)
struct DictNoValueTree {};
template <bool Sorted, typename U> struct DictValueTree {
	using type = DictNoValueTree;
};
template <typename U> struct DictValueTree<true, U> {
	using type = AVLTree<Pair<U, ull>, ull>;
};

template <typename T, typename U>
  requires requires(T x, T y) {
	  { x > y } -> std::convertible_to<bool>;
	  { x < y } -> std::convertible_to<bool>;
	  { x == y } -> std::convertible_to<bool>;
  }
class Dict {

public:
	// Pair's defaulted <=> needs U to have <=>
	static constexpr bool Sorted = std::three_way_comparable<U>;

	U& operator[](const T& key) {
		update_ref();
		ull index;
		if (!indextree.Find(key)) {
			index = (avail_value_indexes.Size > 0) ? avail_value_indexes.Pop()
			                                       : values.Length;
			indextree.Push(key, index);
			if constexpr (Sorted)
				valuetree.Push(Pair<U, ull>(U(), index), index);
			values[index] = U();
			keys[index] = key;
			++Size;
		} else {
			index = indextree.Data();
		}
		return *(create_ref(index));
	}
	bool Has(const T& key) {
		if (search_succeeded && key == keys[search_index])
			return true;
		if (indextree.Find(key)) {
			search_succeeded = true;
			search_index = indextree.Data();
			return true;
		}
		return false;
	}
	void Delete() {
		assert(search_succeeded);
		if constexpr (Sorted) {
			if (referred != nullptr && referred_index == search_index) {
				valuetree.Delete(Pair<U, ull>(before_change, search_index));
				referred = nullptr;
			} else {
				valuetree.Delete(Pair<U, ull>(values[search_index], search_index));
			}
		}
		indextree.Delete(keys[search_index]);
		avail_value_indexes.Push(search_index);
		search_succeeded = false;
		--Size;
	}
	void Delete(const T& key) {
		// Has sets search_index, so it must run even under NDEBUG
		[[maybe_unused]] bool found = Has(key);
		assert(found);
		Delete();
	}
	U& Min()
	  requires Sorted
	{
		update_ref();
		assert(Size > 0);
		valuetree.Nth(0);
		search_succeeded = true;
		search_index = valuetree.Data();
		return *(create_ref(search_index));
	}
	T MinKey()
	  requires Sorted
	{
		Min();
		return keys[search_index];
	}
	U& Max()
	  requires Sorted
	{
		update_ref();
		assert(Size > 0);
		valuetree.Nth(Size - 1);
		search_succeeded = true;
		search_index = valuetree.Data();
		return *(create_ref(search_index));
	}
	T MaxKey()
	  requires Sorted
	{
		Max();
		return keys[search_index];
	}

	LightArray<T> Keys() { return indextree.IndexArray(); }

	ull Size = 0;

private:
	// the previous query is valid unless the index is deleted
	ull search_index;
	bool search_succeeded = false;

	Array<T> keys;
	Array<U> values;
	Stack<ull> avail_value_indexes;

	AVLTree<T, ull> indextree;

	// only used when Sorted
	typename DictValueTree<Sorted, U>::type valuetree;

	U* referred = nullptr;
	ull referred_index;

	U before_change;

	void update_ref() {
		if constexpr (Sorted) {
			if (referred == nullptr || before_change == *referred)
				return;
			valuetree.Find(Pair<U, ull>(before_change, referred_index));
			valuetree.Delete();
			valuetree.Push(Pair<U, ull>(*referred, referred_index), referred_index);
			before_change = *referred;
		}
	}
	U* create_ref(ull index) {
		referred = &values[index];
		referred_index = index;
		if constexpr (Sorted)
			before_change = *referred;
		return referred;
	}
};
