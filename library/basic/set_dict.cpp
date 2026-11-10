#pragma once
#include "balanced_tree.cpp"
#include "base.cpp"
#include "light_array.cpp"
#include <compare>

// valuetree's type is named only when the values can be ordered, so that
// AVLTree<Pair<U, T>, ull> is never formed for an unordered U (e.g. Mod)
struct DictNoValueTree {};
template <bool UseValueTree, typename U, typename T> struct DictValueTree {
	using type = DictNoValueTree;
};
template <typename U, typename T> struct DictValueTree<true, U, T> {
	using type = AVLTree<Pair<U, T>, ull>;
};

// ValueOrder = true enables Min/Max/MinKey/MaxKey/Nth, at the cost of keeping
// every value in a second tree (extra tree walks on each value change)
template <typename T, typename U, bool ValueOrder = false>
  requires requires(T x, T y) {
	  { x > y } -> std::convertible_to<bool>;
	  { x < y } -> std::convertible_to<bool>;
	  { x == y } -> std::convertible_to<bool>;
  }
class Dict {

public:
	Dict() {
		for (auto&& ind : cached_indexes)
			ind = Infty<ull>;
	}

	// valuetree keeps copies of (value, key) in Pair<U, T>, ordered by value and
	// then by key; Pair's defaulted <=> needs both U and T to have <=>
	static_assert(
	  !ValueOrder || (std::three_way_comparable<U> && std::copyable<U> &&
	                  std::three_way_comparable<T> && std::copyable<T>),
	  "Dict<T, U, true> needs U and T to be three-way comparable and copyable"
	);
	static constexpr bool UseValueTree = ValueOrder;

	U& operator[](const T& key) {
		update_ref();
		if (!Has(key)) {
			search_index = (avail_value_indexes.Size > 0) ? avail_value_indexes.Pop() : values.Length;
			indextree.Push(key, search_index);
			if constexpr (UseValueTree) {
				Pair<U, T> added(U(), key);
				valuetree.Push(added, search_index);
				if (min_index != Infty<ull> && added < value_key(min_index))
					min_index = search_index;
				if (max_index != Infty<ull> && value_key(max_index) < added)
					max_index = search_index;
			}
			// [] here: a new search_index may be values.Length, so the arrays must
			// grow
			values[search_index] = U();
			keys[search_index] = key;
			register_cache(search_index);
			++Size;
		}
		return *(create_ref(search_index));
	}
	bool Has(const T& key) {
		if (search_index != Infty<ull> && keys(search_index) == key)
			return true;
		for (ull i = 0; i < SearchCacheSize; ++i) {
			if (cached_indexes[i] != Infty<ull> && keys(cached_indexes[i]) == key) {
				search_index = cached_indexes[i];
				return true;
			}
		}

		if (indextree.Find(key)) {
			search_index = indextree.Data();
			register_cache(search_index);
			return true;
		}
		return false;
	}
	void Delete() {
		assert(search_index != Infty<ull>);
		if constexpr (UseValueTree) {
			if (referred != nullptr && referred_index == search_index) {
				valuetree.Delete(Pair<U, T>(before_change, keys(search_index)));
				referred = nullptr;
			} else {
				valuetree.Delete(value_key(search_index));
			}
		}
		if (min_index == search_index)
			min_index = Infty<ull>;
		if (max_index == search_index)
			max_index = Infty<ull>;
		for (auto&& c : cached_indexes) {
			if (c == search_index)
				c = Infty<ull>;
		}

		indextree.Delete(keys(search_index));
		avail_value_indexes.Push(search_index);
		search_index = Infty<ull>;
		--Size;
	}
	void Delete(const T& key) {
		// Has sets search_index, so it must run even under NDEBUG
		[[maybe_unused]] bool found = Has(key);
		assert(found);
		Delete();
	}
	U& Min()
	  requires UseValueTree
	{
		update_ref();
		assert(Size > 0);
		if (min_index == Infty<ull>) {
			valuetree.Nth(0);
			min_index = valuetree.Data();
		}
		return *(create_ref(min_index));
	}
	T MinKey()
	  requires UseValueTree
	{
		Min();
		return keys(min_index);
	}
	U& Max()
	  requires UseValueTree
	{
		update_ref();
		assert(Size > 0);
		if (max_index == Infty<ull>) {
			valuetree.Nth(Size - 1);
			max_index = valuetree.Data();
		}
		return *(create_ref(max_index));
	}
	T MaxKey()
	  requires UseValueTree
	{
		Max();
		return keys(max_index);
	}
	// the n-th smallest value (0-indexed), ties broken by key
	U& Nth(ull n)
	  requires UseValueTree
	{
		update_ref();
		assert(n < Size);
		valuetree.Nth(n);
		return *(create_ref(valuetree.Data()));
	}

	LightArray<T> Keys() { return indextree.IndexArray(); }

	ull Size = 0;

private:
	// index of the previous Has/[] result, Infty<ull> if none;
	// valid unless the index is deleted
	ull search_index = Infty<ull>;

	// indexes of recently found keys, Infty<ull> = empty slot
	static constexpr ull SearchCacheSize = 5;
	ull cached_indexes[SearchCacheSize];
	ull cache_loc = 0;

	void register_cache(ull ind) {
		cached_indexes[cache_loc] = ind;
		++cache_loc;
		if (cache_loc == SearchCacheSize)
			cache_loc = 0;
	}

	// indexes of the current Min/Max entries, Infty<ull> = unknown
	ull min_index = Infty<ull>;
	ull max_index = Infty<ull>;

	Array<T> keys;
	Array<U> values;
	Stack<ull> avail_value_indexes;

	AVLTree<T, ull> indextree;

	// only used when UseValueTree
	typename DictValueTree<UseValueTree, U, T>::type valuetree;

	// the valuetree key of the entry at index
	Pair<U, T> value_key(ull index) { return Pair<U, T>(values(index), keys(index)); }

	U* referred = nullptr;
	ull referred_index;

	U before_change;

	void update_ref() {
		if constexpr (UseValueTree) {
			if (referred == nullptr || before_change == *referred)
				return;
			// keep min_index/max_index consistent with valuetree's order,
			// which compares (value, key) pairs
			Pair<U, T> changed(*referred, keys(referred_index));
			if (min_index != Infty<ull>) {
				if (referred_index == min_index) {
					if (before_change < *referred)
						min_index = Infty<ull>;
				} else if (changed < value_key(min_index)) {
					min_index = referred_index;
				}
			}
			if (max_index != Infty<ull>) {
				if (referred_index == max_index) {
					if (*referred < before_change)
						max_index = Infty<ull>;
				} else if (value_key(max_index) < changed) {
					max_index = referred_index;
				}
			}

			valuetree.Find(Pair<U, T>(before_change, keys(referred_index)));
			valuetree.Delete();
			valuetree.Push(changed, referred_index);
			before_change = *referred;
		}
	}
	U* create_ref(ull index) {
		referred = &values(index);
		referred_index = index;
		if constexpr (UseValueTree)
			before_change = *referred;
		return referred;
	}
};
