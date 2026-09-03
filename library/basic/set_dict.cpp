#pragma once
#include "balanced_tree.cpp"
#include "base.cpp"
#include "light_array.cpp"

template <typename T, typename U>
  requires requires(T x, T y) {
	  { x > y } -> std::convertible_to<bool>;
	  { x < y } -> std::convertible_to<bool>;
	  { x == y } -> std::convertible_to<bool>;
  }
class Dict {
	AVLTree<T, U> avltree = AVLTree<T, U>(Type<T>, Type<U>);

public:
	ull Size = 0;
	bool search_succeeded = false;
	T prev_search;
	Dict() {};
	bool Has(T ind) {
		if (search_succeeded && ind == prev_search)
			return true;
		search_succeeded = avltree.Find(ind);
		prev_search = ind;
		return search_succeeded;
	}
	U& operator[](T ind) {
		if (search_succeeded && ind == prev_search)
			return avltree.Data();
		search_succeeded = true;
		prev_search = ind;
		if (avltree.Find(ind)) {
			return avltree.Data();
		}
		++Size;
		avltree.Push(ind, U());
		avltree.Find(ind);
		return avltree.Data();
	}

	LightArray<T> Keys() { return avltree.IndexArray(); }
};
