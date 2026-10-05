#pragma once
#include "../basic.cpp"

// T: monoid
// AddFunc: (T, T) -> T, or ((val, block length), (val, block length)) -> T
template <typename T, typename AddFunc> class SegmentTree {
	static_assert(
	  FunctionConcept<AddFunc, T, T, T> ||
	  FunctionConcept<AddFunc, T, Pair<T, ull>, Pair<T, ull>>
	);
	static_assert(!(
	  FunctionConcept<AddFunc, T, T, T> &&
	  FunctionConcept<AddFunc, T, Pair<T, ull>, Pair<T, ull>>
	));

public:
	// init_val is non-deduced so a literal like 0 doesn't fight Type<T> in CTAD
	SegmentTree(
	  TypeVar<T>,
	  const AddFunc& add,
	  ull N,
	  std::type_identity_t<T> init_val = T()
	)
	    : add(add) {
		Array<T> arr = Array<T>();
		Init(N, arr, init_val);
	}
	SegmentTree(TypeVar<T>, const AddFunc& add, Array<T>& arr) : add(add) {
		Init(arr.Length, arr, T());
	}

	T Eval(ull, ull); // evaluate [a,b]
	void Set(ull, T);

	size_t Length;
	size_t CellLength;
	size_t Degree;

	AddFunc add;

	LightArray<T> Val;

	// internal implemention

	void Init(ull N, Array<T>& arr, T init_val);

	T Add(T val1, ull len1, T val2, ull len2);
	ull CellSize(ull cell) { return Length >> Log2(cell + 1); }

	ull CellIndex(size_t start, size_t logsize);

	LightArray<ull> GetRange(size_t start, size_t end);
};

// calls add, passing each side's block length if AddFunc takes them
template <typename T, typename AddFunc>
T SegmentTree<T, AddFunc>::Add(T val1, ull len1, T val2, ull len2) {
	if constexpr (FunctionConcept<AddFunc, T, T, T>) {
		return add(val1, val2);
	} else {
		return add(Pair<T, ull>(val1, len1), Pair<T, ull>(val2, len2));
	}
}

template <typename T, typename AddFunc>
void SegmentTree<T, AddFunc>::Init(ull N, Array<T>& arr, T init_val) {
	Length = BiggerPower2(N);
	CellLength = Length * 2 - 1;
	Degree = Log2(Length * 2);
	Val.Allocate(CellLength);
	for (ull i = 0; i < Length; ++i) {
		if (i < arr.Length) {
			Val[i + Length - 1] = arr[i];
		} else {
			Val[i + Length - 1] = init_val;
		}
	}

	if (Degree == 1)
		return;
	for (ull d = Degree - 2;; --d) {
		ull cell_start = (1ull << (d)) - 1;
		ull cell_end = (1ull << (d + 1)) - 2;
		ull child_len = Length >> (d + 1);
		for (ull i = cell_start; i <= cell_end; ++i) {
			Val[i] = Add(Val[Child(i)], child_len, Val[Child(i) + 1], child_len);
		}
		if (d == 0)
			break;
	}
	return;
}

template <typename T, typename AddFunc>
void SegmentTree<T, AddFunc>::Set(ull point, T val) {
	ull loc = Length + point - 1;
	Val[loc] = val;
	if (loc == 0)
		return;
	loc = Parent(loc);
	while (true) {
		ull child_len = CellSize(Child(loc));
		Val(loc) = Add(Val(Child(loc)), child_len, Val(Child(loc) + 1), child_len);
		if (loc == 0)
			break;
		loc = Parent(loc);
	}
}
template <typename T, typename AddFunc>
T SegmentTree<T, AddFunc>::Eval(size_t start, size_t end) {
	LightArray<ull> range = GetRange(start, end);
	// range is in left-to-right order, so a non-commutative add folds correctly
	T counter = Val[range[0]];
	ull counter_len = CellSize(range[0]);
	for (ull i = 1; i < range.Length; ++i) {
		ull cell_len = CellSize(range[i]);
		counter = Add(counter, counter_len, Val[range[i]], cell_len);
		counter_len += cell_len;
	}
	return counter;
}

template <typename T, typename AddFunc>
ull SegmentTree<T, AddFunc>::CellIndex(size_t start, size_t logsize) {
	return ((1 << (Degree - logsize - 1)) - 1) + (start >> logsize);
}

template <typename T, typename AddFunc>
LightArray<ull> SegmentTree<T, AddFunc>::GetRange(size_t start, size_t end) {
	// each of the two passes takes at most one cell per level
	LightArray<ull> res(2 * Degree);
	ull res_len = 0;
	size_t loc = start;
	for (size_t i = 0; true; ++i) {
		if ((1 << i) + loc > end + 1)
			break;
		if ((1 << i) & loc) {
			res[res_len] = CellIndex(loc, i);
			++res_len;
			loc = (1 << i) + loc;
		}
	}
	if (loc != end + 1) {
		for (size_t i = Log2(SmallerPower2(end + 1 - loc)); true; --i) {
			if ((1 << i) + loc > end + 1)
				continue;
			res[res_len] = CellIndex(loc, i);
			++res_len;
			loc = (1 << i) + loc;
			if (loc == end + 1)
				break;
		}
	}
	res.Length = res_len;
	return res;
}

// T: value, U: action
// AddFunc: (T, T) -> T, or ((val, block length), (val, block length)) -> T
// Func: (U, T) -> T, or (U, (val, block length)) -> T
template <
  typename T,
  typename U,
  typename AddFunc,
  typename Func,
  typename ConvoluteFunc>
class LazySegmentTree {
	static_assert(std::is_copy_assignable<T>::value);
	static_assert(std::is_copy_assignable<U>::value);
	static_assert(
	  FunctionConcept<AddFunc, T, T, T> ||
	  FunctionConcept<AddFunc, T, Pair<T, ull>, Pair<T, ull>>
	);
	static_assert(!(
	  FunctionConcept<AddFunc, T, T, T> &&
	  FunctionConcept<AddFunc, T, Pair<T, ull>, Pair<T, ull>>
	));
	static_assert(
	  FunctionConcept<Func, T, U, T> || FunctionConcept<Func, T, U, Pair<T, ull>>
	);
	static_assert(!(
	  FunctionConcept<Func, T, U, T> && FunctionConcept<Func, T, U, Pair<T, ull>>
	));

public:
	LazySegmentTree(
	  TypeVar<T>,
	  TypeVar<U>,
	  ull N,
	  std::type_identity_t<T> init_val,
	  const AddFunc& add,
	  const Func& func,
	  const ConvoluteFunc& convolute
	)
	    : add(add), func(func), convolute(convolute) {
		Init(N, init_val);
	}

	T Eval(ull, ull); // evaluate [a,b]
	void Act(U, ull, ull);
	void Set(ull, T);

	ull Length;
	ull CellLength;
	ull Degree;

	AddFunc add;
	Func func;
	ConvoluteFunc convolute; // f,g -> f*g

	LightArray<T> Val;
	LightArray<U> Actions;
	LightArray<bool> ActIsNull;
	LightArray<ull> CellStart;
	LightArray<ull> CellEnd;

private:
	void Init(ull N, T init_val);
	T Add(T val1, ull len1, T val2, ull len2);
	T Apply(U action, ull loc);
	ull CellSize(ull loc) { return CellEnd[loc] - CellStart[loc] + 1; }

	void Propagate(ull);
	void Execute(ull);
	void Refresh(ull);
	template <FunctionConcept<void, ull> OnTarget>
	void RangeDFS(ull loc, ull start, ull end, const OnTarget& on_target);
};

template <
  typename T,
  typename U,
  typename AddFunc,
  typename Func,
  typename ConvoluteFunc>
void LazySegmentTree<T, U, AddFunc, Func, ConvoluteFunc>::Init(
  ull N, T init_val
) {
	Length = BiggerPower2(N);
	CellLength = Length * 2 - 1;
	Degree = Log2(Length * 2);
	Val.Allocate(CellLength);
	Actions.Allocate(CellLength);
	ActIsNull.Allocate(CellLength);
	ActIsNull.Set(true);
	CellStart.Allocate(CellLength);
	CellEnd.Allocate(CellLength);

	for (ull i = 0; i < Length; ++i) {
		Val[i + Length - 1] = init_val;
		CellStart[i + Length - 1] = i;
		CellEnd[i + Length - 1] = i;
	}
	if (Degree == 1)
		return;
	for (ull d = Degree - 2;; --d) {
		ull cell_start = (1ull << (d)) - 1;
		ull cell_end = (1ull << (d + 1)) - 2;
		ull child_len = Length >> (d + 1);
		for (ull i = cell_start; i <= cell_end; ++i) {
			Val[i] = Add(Val[Child(i)], child_len, Val[Child(i) + 1], child_len);
			CellStart[i] = (i - cell_start) << (Degree - d - 1);
			CellEnd[i] = ((i - cell_start + 1) << (Degree - d - 1)) - 1;
		}
		if (d == 0)
			break;
	}
	return;
}

// calls add, passing each side's block length if AddFunc takes them
template <
  typename T,
  typename U,
  typename AddFunc,
  typename Func,
  typename ConvoluteFunc>
T LazySegmentTree<T, U, AddFunc, Func, ConvoluteFunc>::Add(
  T val1, ull len1, T val2, ull len2
) {
	if constexpr (FunctionConcept<AddFunc, T, T, T>) {
		return add(val1, val2);
	} else {
		return add(Pair<T, ull>(val1, len1), Pair<T, ull>(val2, len2));
	}
}

// applies action to Val[loc], passing the block length if Func takes it
template <
  typename T,
  typename U,
  typename AddFunc,
  typename Func,
  typename ConvoluteFunc>
T LazySegmentTree<T, U, AddFunc, Func, ConvoluteFunc>::Apply(
  U action, ull loc
) {
	if constexpr (FunctionConcept<Func, T, U, T>) {
		return func(action, Val[loc]);
	} else {
		return func(action, Pair<T, ull>(Val[loc], CellSize(loc)));
	}
}

template <
  typename T,
  typename U,
  typename AddFunc,
  typename Func,
  typename ConvoluteFunc>
inline void
LazySegmentTree<T, U, AddFunc, Func, ConvoluteFunc>::Propagate(ull loc) {
	if (ActIsNull[loc])
		return;
	ActIsNull[loc] = true;
	if (Child(loc) < CellLength) {
		if (!ActIsNull[Child(loc)])
			Actions[Child(loc)] = convolute(Actions[loc], Actions[Child(loc)]);
		else {
			Actions[Child(loc)] = Actions[loc];
			ActIsNull[Child(loc)] = false;
		}
		if (!ActIsNull[Child(loc) + 1])
			Actions[Child(loc) + 1] =
			  convolute(Actions[loc], Actions[Child(loc) + 1]);
		else {
			Actions[Child(loc) + 1] = Actions[loc];
			ActIsNull[Child(loc) + 1] = false;
		}
	}
}

template <
  typename T,
  typename U,
  typename AddFunc,
  typename Func,
  typename ConvoluteFunc>
inline void
LazySegmentTree<T, U, AddFunc, Func, ConvoluteFunc>::Execute(ull loc) {
	if (ActIsNull[loc])
		return;
	Val[loc] = Apply(Actions[loc], loc);
	Propagate(loc);
}

template <
  typename T,
  typename U,
  typename AddFunc,
  typename Func,
  typename ConvoluteFunc>
inline void
LazySegmentTree<T, U, AddFunc, Func, ConvoluteFunc>::Refresh(ull loc) {
	if (Child(loc) >= CellLength)
		return;
	ull child_len = CellSize(Child(loc));
	Val[loc] = Add(Val[Child(loc)], child_len, Val[Child(loc) + 1], child_len);
}

// visits the cells covering [start, end] left to right, calling on_target(loc)
// on each; cells outside the range are executed, and partial cells are
// propagated before and refreshed after their children
// on_target must call Execute(loc): the parent's Refresh reads Val[loc], and
// loc's pending action (just propagated down) isn't applied to it otherwise
template <
  typename T,
  typename U,
  typename AddFunc,
  typename Func,
  typename ConvoluteFunc>
template <FunctionConcept<void, ull> OnTarget>
void LazySegmentTree<T, U, AddFunc, Func, ConvoluteFunc>::RangeDFS(
  ull loc, ull start, ull end, const OnTarget& on_target
) {
	if (CellEnd[loc] < start || end < CellStart[loc]) {
		Execute(loc);
		return;
	}
	if (start <= CellStart[loc] && end >= CellEnd[loc]) {
		on_target(loc);
		return;
	}
	Propagate(loc);
	RangeDFS(Child(loc), start, end, on_target);
	RangeDFS(Child(loc) + 1, start, end, on_target);
	Refresh(loc);
}

template <
  typename T,
  typename U,
  typename AddFunc,
  typename Func,
  typename ConvoluteFunc>
inline T
LazySegmentTree<T, U, AddFunc, Func, ConvoluteFunc>::Eval(ull start, ull end) {
	ull res_len = 0;
	T res;
	RangeDFS(0, start, end, [&](ull loc) {
		Execute(loc);
		if (res_len == 0) {
			res = Val[loc];
		} else {
			res = Add(res, res_len, Val[loc], CellSize(loc));
		}
		res_len += CellSize(loc);
	});
	return res;
}

template <
  typename T,
  typename U,
  typename AddFunc,
  typename Func,
  typename ConvoluteFunc>
inline void LazySegmentTree<T, U, AddFunc, Func, ConvoluteFunc>::Act(
  U action, ull start, ull end
) {
	RangeDFS(0, start, end, [&](ull loc) {
		if (ActIsNull[loc]) {
			Actions[loc] = action;
		} else {
			Actions[loc] = convolute(action, Actions[loc]);
		}
		ActIsNull[loc] = false;
		Execute(loc);
	});
}

template <
  typename T,
  typename U,
  typename AddFunc,
  typename Func,
  typename ConvoluteFunc>
inline void
LazySegmentTree<T, U, AddFunc, Func, ConvoluteFunc>::Set(ull loc, T val) {
	auto on_target = [&](ull target) {
		Execute(target);
		Val[target] = val;
	};
	RangeDFS(0, loc, loc, on_target);
}
