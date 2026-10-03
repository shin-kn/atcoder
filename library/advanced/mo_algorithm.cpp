#pragma once
#include "../standard.cpp"

template <typename ArrayType>
  requires ArrayLike<ArrayType, Tuple<ull, 2>>
LightArray<Tuple<ull, 3>> MoSort(ull sqrt_n, ArrayType& arr) {
	// this is related to Mo's algorithm
	// arr[i]=(l,r); returns (l,r,idx) where idx=i, so answers can be put back in
	// query order
	assert(sqrt_n > 0);
	using tuple = Tuple<ull, 3>;
	Array<Array<tuple>> each_div;
	for (ull i = 0; i < arr.Length; ++i) {
		each_div[arr[i][0] / sqrt_n].Push(tuple(arr[i][0], arr[i][1], i));
	}
	auto ltop = [](tuple v1, tuple v2) -> bool { return v1[1] < v2[1]; };
	auto gtop = [](tuple v1, tuple v2) -> bool { return v1[1] > v2[1]; };
	// zigzag: r ascending in even blocks, descending in odd blocks
	for (ull i = 0; i < each_div.Length; ++i) {
		if (i % 2 == 0)
			each_div[i] = Sort(each_div[i], ltop);
		else
			each_div[i] = Sort(each_div[i], gtop);
	}
	LightArray<tuple> res(arr.Length);
	ull counter = 0;
	for (ull i = 0; i < each_div.Length; ++i) {
		for (ull j = 0; j < each_div[i].Length; ++j) {
			res[counter] = each_div[i][j];
			++counter;
		}
	}
	return res;
}

template <
  typename ArrayType,
  FunctionConcept<void, ull> PushFunc,
  FunctionConcept<void, ull> PopFunc,
  FunctionConcept<void, Tuple<ull, 3>> EvalFunc>
  requires ArrayLike<ArrayType, Tuple<ull, 2>>
void MoAlgorithm(
  ArrayType& arr, const PushFunc& push, const PopFunc& pop, const EvalFunc& eval
) {
	// arr: [(l,r),...]
	ull N = 0;
	for (ull i = 0; i < arr.Length; ++i)
		UpdateMax(N, arr[i][1] + 1);
	ull Q = arr.Length;
	if (Q == 0)
		return;

	using tuple = Tuple<ull, 3>;
	LightArray<tuple> ranges = MoSort(Max(Sqrt(N * N / Q), ull(1)), arr);
	// [(l,r,ind),...]
	if (ranges.Length == 0)
		return;
	ull l = ranges[0][0];
	ull r = ranges[0][0];
	push(l);
	for (ull i = 0; i < ranges.Length; ++i) {
		tuple range = ranges[i];
		if (range[0] < l) {
			for (ull j = l - 1;; --j) {
				push(j);
				if (j == range[0])
					break;
			}
			l = range[0];
		}
		if (r < range[1]) {
			for (ull j = r + 1; j <= range[1]; ++j) {
				push(j);
			}
			r = range[1];
		}
		if (l < range[0]) {
			for (ull j = l; j < range[0]; ++j) {
				pop(j);
			}
			l = range[0];
		}
		if (r > range[1]) {
			for (ull j = r; j > range[1]; --j) {
				pop(j);
			}
			r = range[1];
		}
		eval(range);
	}
}
