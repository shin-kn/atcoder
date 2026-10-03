#pragma once
#include "array.cpp"
#include "base.cpp"
#include "function_type.cpp"
#include "template_functions.cpp"
#include "utils.cpp"

template <typename T, typename U, bool Smaller = true> class Heap {
	Array<T> arr;
	Array<U> data;
	void Slide(size_t from, size_t to);

public:
	size_t Size = 0;
	void Push(T, U);
	U Pop();
	T Top();
};

template <typename T, typename U, bool Smaller>
inline void Heap<T, U, Smaller>::Slide(size_t from, size_t to) {
	arr[to] = arr[from];
	data[to] = data[from];
}

template <typename T, typename U, bool Smaller>
inline void Heap<T, U, Smaller>::Push(T n1, U n2) { // log N
	size_t loc = this->Size;
	while (loc != 0) {
		if constexpr (Smaller) {
			if (arr[Parent(loc)] > n1) {
				Slide(Parent(loc), loc);
				loc = Parent(loc);
			} else {
				break;
			}
		} else {
			if (arr[Parent(loc)] < n1) {
				Slide(Parent(loc), loc);
				loc = Parent(loc);
			} else {
				break;
			}
		}
	}
	arr[loc] = n1;
	data[loc] = n2;
	++this->Size;
}

template <typename T, typename U, bool Smaller>
inline T Heap<T, U, Smaller>::Top() { // log N
	return arr[0];
}

template <typename T, typename U, bool Smaller>
inline U Heap<T, U, Smaller>::Pop() { // log N
	if (Size == 0)
		return U();
	U poped = data(0);
	--Size;
	if (Size == 0)
		return poped;
	size_t loc = 0;
	while (true) {
		size_t bestloc = Size;
		if (Child(loc) < Size) {
			if constexpr (Smaller) {
				if (arr(Child(loc)) < arr(bestloc))
					bestloc = Child(loc);
			} else {
				if (arr(Child(loc)) > arr(bestloc))
					bestloc = Child(loc);
			}
		}
		if (Child(loc) + 1 < Size) {
			if constexpr (Smaller) {
				if (arr(Child(loc) + 1) < arr(bestloc))
					bestloc = Child(loc) + 1;
			} else {
				if (arr(Child(loc) + 1) > arr(bestloc))
					bestloc = Child(loc) + 1;
			}
		}
		if (bestloc == Size) {
			Slide(Size, loc);
			break;
		}
		Slide(bestloc, loc);
		loc = bestloc;
	}
	return poped;
}

template <typename T, bool Smaller = true> class SimpleHeap {
	Array<T> arr;
	void Slide(size_t from, size_t to);

public:
	size_t Size = 0;
	void Push(T);
	T Pop();
	T Top();
};

template <typename T, bool Smaller>
inline void SimpleHeap<T, Smaller>::Slide(size_t from, size_t to) {
	arr[to] = arr[from];
}

template <typename T, bool Smaller>
inline void SimpleHeap<T, Smaller>::Push(T n1) { // log N
	size_t loc = this->Size;
	while (loc != 0) {
		if constexpr (Smaller) {
			if (arr[Parent(loc)] > n1) {
				Slide(Parent(loc), loc);
				loc = Parent(loc);
			} else {
				break;
			}
		} else {
			if (arr[Parent(loc)] < n1) {
				Slide(Parent(loc), loc);
				loc = Parent(loc);
			} else {
				break;
			}
		}
	}
	arr[loc] = n1;
	++this->Size;
}

template <typename T, bool Smaller>
inline T SimpleHeap<T, Smaller>::Top() { // log N
	return arr[0];
}

template <typename T, bool Smaller>
inline T SimpleHeap<T, Smaller>::Pop() { // log N
	if (Size == 0)
		return T();
	T poped = arr[0];
	--Size;
	if (Size == 0)
		return poped;
	size_t loc = 0;
	while (true) {
		size_t bestloc = Size;
		if (Child(loc) < Size) {
			if constexpr (Smaller) {
				if (arr[Child(loc)] < arr[bestloc])
					bestloc = Child(loc);
			} else {
				if (arr[Child(loc)] > arr[bestloc])
					bestloc = Child(loc);
			}
		}
		if (Child(loc) + 1 < Size) {
			if constexpr (Smaller) {
				if (arr[Child(loc) + 1] < arr[bestloc])
					bestloc = Child(loc) + 1;
			} else {
				if (arr[Child(loc) + 1] > arr[bestloc])
					bestloc = Child(loc) + 1;
			}
		}
		if (bestloc == Size) {
			Slide(Size, loc);
			break;
		}
		Slide(bestloc, loc);
		loc = bestloc;
	}
	return poped;
}

// top is the smallest element w.r.t. ltop (pass GTOp<T> for a max-heap)
template <
  typename T,
  FunctionConcept<bool, T, T> LTFunc = std::remove_cvref_t<decltype(LTOp<T>)>>
class FastHeap {
	T* arr = nullptr;
	void Slide(size_t from, size_t to);
	LTFunc ltop;

public:
	size_t Size = 0;
	void Push(T);
	FastHeap(ull n, const LTFunc& ltop = LTOp<T>) : ltop(ltop) { Allocate(n); }
	void Allocate(size_t n) {
		n = BiggerPower2(n);
		arr = new T[n];
	}
	FastHeap(FastHeap& src) = delete;
	FastHeap& operator=(FastHeap& src) = delete;
	~FastHeap() {
		if (arr != nullptr) {
			delete[] arr;
		}
	}
	T Pop();
	T Top();
};

template <typename T, FunctionConcept<bool, T, T> LTFunc>
inline void FastHeap<T, LTFunc>::Slide(size_t from, size_t to) {
	arr[to] = std::move(arr[from]);
}

template <typename T, FunctionConcept<bool, T, T> LTFunc>
inline void FastHeap<T, LTFunc>::Push(T n1) { // log N
	size_t loc = this->Size;
	while (loc != 0) {
		if (ltop(n1, arr[Parent(loc)])) {
			Slide(Parent(loc), loc);
			loc = Parent(loc);
		} else {
			break;
		}
	}
	arr[loc] = std::move(n1);
	++this->Size;
}

template <typename T, FunctionConcept<bool, T, T> LTFunc>
inline T FastHeap<T, LTFunc>::Top() { // log N
	return arr[0];
}

template <typename T, FunctionConcept<bool, T, T> LTFunc>
inline T FastHeap<T, LTFunc>::Pop() { // log N
	if (Size == 0)
		return T();
	T poped = std::move(arr[0]);
	--Size;
	if (Size == 0)
		return poped;
	size_t loc = 0;
	while (true) {
		size_t bestloc = Size;
		if (Child(loc) < Size) {
			if (ltop(arr[Child(loc)], arr[bestloc]))
				bestloc = Child(loc);
		}
		if (Child(loc) + 1 < Size) {
			if (ltop(arr[Child(loc) + 1], arr[bestloc]))
				bestloc = Child(loc) + 1;
		}
		if (bestloc == Size) {
			Slide(Size, loc);
			break;
		}
		Slide(bestloc, loc);
		loc = bestloc;
	}
	return poped;
}
