#pragma once
#include "balanced_tree.cpp"
#include "base.cpp"

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
	Dict() {};
	bool Has(T ind) { return avltree.Find(ind); }
	U& operator[](T ind) {
		if (avltree.Find(ind)) {
			return avltree.Data();
		}
		++Size;
		avltree.Push(ind, U());
		avltree.Find(ind);
		return avltree.Data();
	}

	Array<T> Keys() { return avltree.IndexArray(); }
};
