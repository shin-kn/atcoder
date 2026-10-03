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

template <typename ArrayType>
  requires ArrayLike<ArrayType, ArrayElement<ArrayType>>
LightArray<ArrayElement<ArrayType>> Sort(ArrayType& arr) {
	return Sort(arr, LTOp<ArrayElement<ArrayType>>);
}

template <
  typename ArrayType,
  FunctionConcept<bool, ArrayElement<ArrayType>, ArrayElement<ArrayType>>
    LTFunc>
  requires ArrayLike<ArrayType, ArrayElement<ArrayType>>
LightArray<ArrayElement<ArrayType>> Sort(ArrayType& arr, const LTFunc& ltop) {
	using T = ArrayElement<ArrayType>;
	FastHeap<T, LTFunc> heap(arr.Length, ltop);
	for (ull i = 0; i < arr.Length; ++i) {
		heap.Push(arr[i]);
	}
	LightArray<T> res(arr.Length);
	for (ull i = 0; i < res.Length; ++i) {
		res[i] = heap.Pop();
	}
	return res;
}

/*
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
*/
