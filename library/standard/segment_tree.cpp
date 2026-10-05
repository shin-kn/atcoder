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
	void GetRange(ull, ull);
	Stack<ull> ToBeExecuted;
	Stack<ull> ToBeRefreshed;
	Queue<ull> TargetRange;
	Stack<ull> get_range_stack; // reused by GetRange to avoid reallocating

	void Propagate(ull);
	void Execute(ull);
	void Refresh(ull);
	void ExecuteAndRefreshAll(); // Of ToBeExecuted and ToBeRefreshed
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
inline void LazySegmentTree<T, U, AddFunc, Func, ConvoluteFunc>::GetRange(
  ull start, ull end
) {
	ToBeExecuted.Clear();
	ToBeRefreshed.Clear();
	TargetRange.Clear();

	// DFS pushes each cell before its descendants, so popping ToBeRefreshed
	// refreshes children before parents
	get_range_stack.Clear();
	get_range_stack.Push(0);
	while (get_range_stack.Size > 0) {
		ull loc = get_range_stack.Pop();
		if (CellEnd[loc] < start || end < CellStart[loc]) {
			ToBeExecuted.Push(loc);
			continue;
		}

		if (start <= CellStart[loc] && end >= CellEnd[loc]) {
			ToBeExecuted.Push(loc);
			TargetRange.Push(loc);
			continue;
		}
		Propagate(loc);
		ToBeRefreshed.Push(loc);
		// right first, so the left child pops first and TargetRange is in order
		get_range_stack.Push(Child(loc) + 1);
		get_range_stack.Push(Child(loc));
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

template <
  typename T,
  typename U,
  typename AddFunc,
  typename Func,
  typename ConvoluteFunc>
inline void
LazySegmentTree<T, U, AddFunc, Func, ConvoluteFunc>::ExecuteAndRefreshAll() {
	while (ToBeExecuted.Size > 0) {
		Execute(ToBeExecuted.Pop());
	}
	while (ToBeRefreshed.Size > 0) {
		Refresh(ToBeRefreshed.Pop());
	}
}

template <
  typename T,
  typename U,
  typename AddFunc,
  typename Func,
  typename ConvoluteFunc>
inline T
LazySegmentTree<T, U, AddFunc, Func, ConvoluteFunc>::Eval(ull start, ull end) {
	GetRange(start, end);
	ExecuteAndRefreshAll();
	ull first = TargetRange.Pop();
	T counter = Val[first];
	ull counter_len = CellSize(first);
	while (TargetRange.Size > 0) {
		ull loc = TargetRange.Pop();
		counter = Add(counter, counter_len, Val[loc], CellSize(loc));
		counter_len += CellSize(loc);
	}
	return counter;
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
	GetRange(start, end);
	while (TargetRange.Size > 0) {
		ull loc = TargetRange.Pop();
		if (ActIsNull[loc]) {
			Actions[loc] = action;
		} else {
			Actions[loc] = convolute(action, Actions[loc]);
		}
		ActIsNull[loc] = false;
	}
	ExecuteAndRefreshAll();
}

template <
  typename T,
  typename U,
  typename AddFunc,
  typename Func,
  typename ConvoluteFunc>
inline void
LazySegmentTree<T, U, AddFunc, Func, ConvoluteFunc>::Set(ull loc, T val) {
	ull loc_cell = Length - 1 + loc;
	GetRange(loc, loc);
	Execute(loc_cell);
	Val[loc_cell] = val;
	ExecuteAndRefreshAll();
}
