
#pragma once
#include "../basic.cpp"
#include "balanced_tree.cpp"

template <typename T, typename U, FunctionConcept<bool, T, T> IndexLT,
          FunctionConcept<bool, U, U> DataLT,
          FunctionConcept<bool, Pair<U, T>, Pair<U, T>> IndexDataLT>
class AVLArrayClass {
	static_assert(std::is_copy_assignable<T>::value);
	static_assert(std::is_copy_assignable<U>::value);

public:
	AVLArrayClass(TypeVar<T>, TypeVar<U>, IndexLT& index_lt, DataLT& data_lt,
	              IndexDataLT& index_data_lt);

	// index updating
	U& operator[](T ind);
	U& Nth(ull ind);
	void Delete();
	void Delete(T ind);
	void DeleteIfExists(T ind);
	// not index updating
	T Index() { return index; }
	bool Exists(T ind);

	// index updating
	U& Min() { return Nth(0); }
	U& Max() { return Nth(Length - 1); }

	ull Length = 0;

	//::Array<T> IndexArray(){return index_tree.IndexArray();}
	//::Array<Pair<T,U>> Array(){return index_tree.Array();}
	::Array<U> DataArray();

	// internal things

	bool queued = false;
	T index;
	U* queuePointer = nullptr;
	U originalValue;

	IndexLT index_lt;
	DataLT data_lt;
	IndexDataLT index_data_lt;

	AVLTree<T, U, IndexLT> index_tree;
	AVLTree<Pair<U, T>, T, IndexDataLT> data_tree;

	void Resolve();

	bool neq(U n1, U n2) { return data_lt(n1, n2) || data_lt(n2, n1); }
};

template <typename T, typename U, FunctionConcept<bool, T, T> IndexLT,
          FunctionConcept<bool, U, U> DataLT,
          FunctionConcept<bool, Pair<U, T>, Pair<U, T>> IndexDataLT>
AVLArrayClass<T, U, IndexLT, DataLT, IndexDataLT>::AVLArrayClass(
    TypeVar<T>, TypeVar<U>, IndexLT& index_lt, DataLT& data_lt,
    IndexDataLT& index_data_lt)
    : index_lt(index_lt), data_lt(data_lt), index_data_lt(index_data_lt),
      index_tree(AVLTree<T, U, IndexLT>(Type<T>, Type<U>, index_lt)),
      data_tree(AVLTree<Pair<U, T>, T, IndexDataLT>(Type<Pair<U, T>>, Type<T>,
                                                    index_data_lt)) {}

template <typename T, typename U, FunctionConcept<bool, T, T> IndexLT,
          FunctionConcept<bool, U, U> DataLT,
          FunctionConcept<bool, Pair<U, T>, Pair<U, T>> IndexDataLT>
::Array<U> AVLArrayClass<T, U, IndexLT, DataLT, IndexDataLT>::DataArray() {
	::Array<U> arr(Length);
	::Array<Pair<T, U>> pair_arr = data_tree.IndexArray();
	for (ull i = 0; i < pair_arr.Length; ++i) {
		arr[i] = pair_arr[i].val1;
	}
	return arr;
}

template <typename T, typename U, FunctionConcept<bool, T, T> IndexLT,
          FunctionConcept<bool, U, U> DataLT,
          FunctionConcept<bool, Pair<U, T>, Pair<U, T>> IndexDataLT>
void AVLArrayClass<T, U, IndexLT, DataLT, IndexDataLT>::Resolve() {
	if (queued) {
		if (neq(originalValue, *queuePointer)) {
			data_tree.Find(Pair<U, T>(originalValue, index));
			data_tree.Delete();
			data_tree.Push(Pair<U, T>(*queuePointer, index), index);
		}
		queued = false;
	}
}

template <typename T, typename U, FunctionConcept<bool, T, T> IndexLT,
          FunctionConcept<bool, U, U> DataLT,
          FunctionConcept<bool, Pair<U, T>, Pair<U, T>> IndexDataLT>
U& AVLArrayClass<T, U, IndexLT, DataLT, IndexDataLT>::operator[](T ind) {
	Resolve();
	index = ind;
	if (!index_tree.Find(ind)) {
		++Length;
		U initval = U();
		index_tree.Push(ind, initval);
		data_tree.Push(Pair<U, T>(initval, index), index);
	}
	originalValue = index_tree.Data();
	queuePointer = &(index_tree.Data());
	queued = true;
	return index_tree.Data();
}

template <typename T, typename U, FunctionConcept<bool, T, T> IndexLT,
          FunctionConcept<bool, U, U> DataLT,
          FunctionConcept<bool, Pair<U, T>, Pair<U, T>> IndexDataLT>
void AVLArrayClass<T, U, IndexLT, DataLT, IndexDataLT>::Delete() {
	assert(queued);
	queued = false;
	--Length;
	index_tree.Find(index);
	index_tree.Delete();
	data_tree.Find(Pair<U, T>(originalValue, index));
	data_tree.Delete();
}

template <typename T, typename U, FunctionConcept<bool, T, T> IndexLT,
          FunctionConcept<bool, U, U> DataLT,
          FunctionConcept<bool, Pair<U, T>, Pair<U, T>> IndexDataLT>
void AVLArrayClass<T, U, IndexLT, DataLT, IndexDataLT>::DeleteIfExists(T ind) {
	if (Exists(ind)) {
		Delete(ind);
	}
}

template <typename T, typename U, FunctionConcept<bool, T, T> IndexLT,
          FunctionConcept<bool, U, U> DataLT,
          FunctionConcept<bool, Pair<U, T>, Pair<U, T>> IndexDataLT>
void AVLArrayClass<T, U, IndexLT, DataLT, IndexDataLT>::Delete(T ind) {
	(*this)[ind];
	Delete();
}

template <typename T, typename U, FunctionConcept<bool, T, T> IndexLT,
          FunctionConcept<bool, U, U> DataLT,
          FunctionConcept<bool, Pair<U, T>, Pair<U, T>> IndexDataLT>
bool AVLArrayClass<T, U, IndexLT, DataLT, IndexDataLT>::Exists(T ind) {
	return index_tree.Find(ind);
}

template <typename T, typename U, FunctionConcept<bool, T, T> IndexLT,
          FunctionConcept<bool, U, U> DataLT,
          FunctionConcept<bool, Pair<U, T>, Pair<U, T>> IndexDataLT>
U& AVLArrayClass<T, U, IndexLT, DataLT, IndexDataLT>::Nth(ull ind) {
	assert(ind < Length);
	Resolve();
	data_tree[ind];
	index = data_tree.Data();
	queued = true;
	index_tree.Find(index);
	originalValue = index_tree.Data();
	queuePointer = &(index_tree.Data());
	return index_tree.Data();
}

template <typename T, typename U, typename IndexLT, typename DataLT>
auto AVLArray(TypeVar<T> t_type, TypeVar<U> u_type, IndexLT&& index_lt,
              DataLT&& data_lt) {
	auto index_data_lt = [=](Pair<U, T> n1, Pair<U, T> n2) -> bool {
		if (data_lt(n1.val1, n2.val1) || data_lt(n2.val1, n1.val1)) {
			return data_lt(n1.val2, n2.val2);
		}
		return index_lt(n1.val2, n2.val2);
	};
	AVLArrayClass<T, U, IndexLT, DataLT, decltype(index_data_lt)> avlarray(
	    t_type, u_type, std::forward<IndexLT>(index_lt),
	    std::forward<DataLT>(data_lt), index_data_lt);
	return avlarray;
}
