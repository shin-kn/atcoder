#pragma once
#include "../basic.cpp"

// Inf,Sup should be
//  exists v in [val1,val2] that func(v)=true

template <typename T> T Sup(T val1, T val2, FunctionType<bool(T)> func) {
	T ngval;
	bool ngvalinit = false;
	T loc;
	while (true) {
		if (ngvalinit) {
			loc = (val1 + ngval) / 2;
			if (loc == ngval || loc == val1)
				break;
		} else {
			loc = (val1 + val2) / 2;
			if (loc == val1) {
				if (val1 == val2)
					break;
				loc = val2;
			}
		}
		if (func(loc)) {
			val1 = loc;
		} else {
			ngvalinit = true;
			ngval = loc;
		}
	}
	return val1;
}

template <typename T, typename Func> T Sup(T val1, T val2, Func&& func) {
	return Sup(val1, val2, FunctionType<bool(T)>(std::forward<Func>(func)));
}

template <typename T> T Inf(T val1, T val2, FunctionType<bool(T)> func) {
	T ngval;
	bool ngvalinit = false;
	T loc;
	while (true) {
		if (ngvalinit) {
			loc = (ngval + val2) / 2;
			if (loc == ngval || loc == val2)
				break;
		} else {
			loc = (val1 + val2) / 2;
			if (loc == val2) {
				if (val1 == val2)
					break;
				loc = val1;
			}
		}
		if (func(loc)) {
			val2 = loc;
		} else {
			ngvalinit = true;
			ngval = loc;
		}
	}
	return val2;
}

template <typename T, typename Func> T Inf(T val1, T val2, Func&& func) {
	return Inf(val1, val2, FunctionType<bool(T)>(std::forward<Func>(func)));
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
Array<T>& arr1, Array<U>& arr2, Array<V>& arr3, bool Smaller = true) {
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
