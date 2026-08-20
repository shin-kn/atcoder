#pragma once
#include "../basic.cpp"

// Inf,Sup should be
//  exists v in [val1,val2] that func(v)=true
template <std::integral Int, FunctionConcept<bool, Int> FuncType>
Int Sup(Int lower, Int upper, FuncType&& func) {
	++upper;
	// func(upper)=false;
	while (lower + Int(1) != upper) {
		Int test = (lower + upper) / Int(2);
		if (func(test)) {
			lower = test;
		} else {
			upper = test;
		}
	}
	return lower;
}

template <std::integral Int, FunctionConcept<bool, Int> FuncType>
Int Inf(Int lower, Int upper, FuncType&& func) {
	--lower;
	// func(lower)=false;
	while (lower + Int(1) != upper) {
		Int test = (lower + upper) / Int(2);
		if (func(test)) {
			upper = test;
		} else {
			lower = test;
		}
	}
	return upper;
}

// assume arraylike is sorted
// assume arr[0]<=val
template <typename T, ArrayLike<T> Arr> ull Leq(Arr& arr, T val) {
	ull lower = 0;
	ull upper = arr.Length;
	// func(upper)=false;
	while (lower + ull(1) != upper) {
		ull test = (lower + upper) / ull(2);
		if (arr[test] <= val) {
			lower = test;
		} else {
			upper = test;
		}
	}
	return lower;
}
// assume arraylike is sorted
// assume arr[arr.Length-1]>=val
template <typename T, ArrayLike<T> Arr> ull Geq(Arr& arr, T val) {
	if (arr[0] >= val)
		return 0;
	ull lower = 0;
	ull upper = arr.Length - 1;
	// func(lower)=false;
	while (lower + ull(1) != upper) {
		ull test = (lower + upper) / ull(2);
		if (arr[test] >= val) {
			upper = test;
		} else {
			lower = test;
		}
	}
	return upper;
}

template <typename T> void Sort(Array<T>& arr, bool Smaller = true) {
	if (Smaller) {
		FastHeap<T, true> heap(arr.Length);
		for (size_t i = 0; i < arr.Length; ++i) {
			heap.Push(arr(i));
		}
		Array<T> res;
		while (heap.Size > 0) {
			res.Push(heap.Pop());
		}
		arr = std::move(res);
	} else {
		FastHeap<T, false> heap(arr.Length);
		for (size_t i = 0; i < arr.Length; ++i) {
			heap.Push(arr(i));
		}
		Array<T> res;
		while (heap.Size > 0) {
			res.Push(heap.Pop());
		}
		arr = std::move(res);
	}
}

template <typename T, typename U>
void SortByKey(Array<T>& arr1, Array<U>& arr2, bool Smaller = true) {
	if (Smaller) {
		Heap<size_t, size_t, true> heap;
		for (size_t i = 0; i < arr1.Length; ++i) {
			heap.Push(arr1(i), i);
		}
		Array<T> res1(arr1.Length);
		Array<U> res2(arr1.Length);
		size_t counter = 0;
		while (heap.Size > 0) {
			size_t loc = heap.Pop();
			res1(counter) = std::move(arr1(loc));
			res2(counter) = std::move(arr2(loc));
			++counter;
		}
		arr1 = std::move(res1);
		arr2 = std::move(res2);
	} else {
		Heap<size_t, size_t, false> heap;
		for (size_t i = 0; i < arr1.Length; ++i) {
			heap.Push(arr1(i), i);
		}
		Array<T> res1(arr1.Length);
		Array<U> res2(arr1.Length);
		size_t counter = 0;
		while (heap.Size > 0) {
			size_t loc = heap.Pop();
			res1(counter) = std::move(arr1(loc));
			res2(counter) = std::move(arr2(loc));
			++counter;
		}
		arr1 = std::move(res1);
		arr2 = std::move(res2);
	}
}
template <typename T, typename U, typename V>
void SortByKey(
  Array<T>& arr1, Array<U>& arr2, Array<V>& arr3, bool Smaller = true
) {
	class Wrapper {
		Array<T>* arr;

	public:
		size_t val;
		Wrapper() {}
		Wrapper(Array<T>* arr, size_t n) : arr(arr), val(n) {}
		inline bool operator<(Wrapper other) {
			return (*arr)(val) < (*arr)(other.val);
		}
		inline bool operator>(Wrapper other) {
			return (*arr)(val) > (*arr)(other.val);
		}
		inline bool operator==(Wrapper other) {
			return (*arr)(val) == (*arr)(other.val);
		}
	};
	if (Smaller) {
		FastHeap<Wrapper, true> heap(arr1.Length);
		for (size_t i = 0; i < arr1.Length; ++i) {
			heap.Push(Wrapper(&arr1, i));
		}
		Array<T> res1(arr1.Length);
		Array<U> res2(arr1.Length);
		Array<V> res3(arr1.Length);
		size_t counter = 0;
		while (heap.Size > 0) {
			size_t loc = heap.Pop().val;
			res1(counter) = std::move(arr1(loc));
			res2(counter) = std::move(arr2(loc));
			res3(counter) = std::move(arr3(loc));
			++counter;
		}
		arr1 = std::move(res1);
		arr2 = std::move(res2);
		arr3 = std::move(res3);
	} else {
		FastHeap<Wrapper, false> heap(arr1.Length);
		for (size_t i = 0; i < arr1.Length; ++i) {
			heap.Push(Wrapper(&arr1, i));
		}
		Array<T> res1(arr1.Length);
		Array<U> res2(arr1.Length);
		Array<V> res3(arr1.Length);
		size_t counter = 0;
		while (heap.Size > 0) {
			size_t loc = heap.Pop().val;
			res1(counter) = std::move(arr1(loc));
			res2(counter) = std::move(arr2(loc));
			res3(counter) = std::move(arr3(loc));
			++counter;
		}
		arr1 = std::move(res1);
		arr2 = std::move(res2);
		arr3 = std::move(res3);
	}
}
