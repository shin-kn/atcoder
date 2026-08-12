#pragma once
#include "../basic.cpp"

// T: monoid
template <typename T> class SegmentTree {
	static_assert(std::is_copy_assignable<T>::value);

public:
	SegmentTree() {}

	template <typename AddFunc> SegmentTree(ull N, AddFunc&& func) {
		Init(N, std::forward<AddFunc>(func));
	}
	template <typename AddFunc> SegmentTree(ull N, T val, AddFunc&& func) {
		Init(N, val, std::forward<AddFunc>(func));
	}
	template <typename AddFunc> SegmentTree(Array<T>& arr, AddFunc&& func) {
		Init(arr, std::forward<AddFunc>(func));
	}

	template <typename AddFunc> void Init(ull N, AddFunc&& func) {
		Init(N, T(), std::forward<AddFunc>(func));
	}
	template <typename AddFunc> void Init(ull N, T val, AddFunc&& func) {
		Array<T> arr = Array<T>();
		Init(N, arr, val, std::forward<AddFunc>(func));
	}
	template <typename AddFunc> void Init(Array<T>& arr, AddFunc&& func) {
		Init(arr.Length, arr, T(), std::forward<AddFunc>(func));
	}
	template <typename AddFunc>
	void Init(ull N, Array<T>& arr, T init_val, AddFunc&& func);

	T Eval(ull, ull); // evaluate [a,b]
	void Set(ull, T);

	size_t Length;
	size_t CellLength;
	size_t Degree;

	FunctionType<T(T, T)> add;

	LightArray<T> Val;

	// internal implemention

	ull CellIndex(size_t start, size_t logsize);

	Array<ull> GetRange(size_t start, size_t end);
};

template <typename T>
template <typename AddFunc>
void SegmentTree<T>::Init(ull N, Array<T>& arr, T init_val, AddFunc&& func) {
	add = FunctionType<T(T, T)>(std::forward<AddFunc>(func));
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
		for (ull i = cell_start; i <= cell_end; ++i) {
			Val[i] = add(Val[Child(i)], Val[Child(i) + 1]);
		}
		if (d == 0)
			break;
	}
	return;
}

template <typename T> void SegmentTree<T>::Set(ull point, T val) {
	ull loc = Length + point - 1;
	Val[loc] = val;
	if (loc == 0)
		return;
	loc = Parent(loc);
	while (true) {
		Val(loc) = add(Val(Child(loc)), Val(Child(loc) + 1));
		if (loc == 0)
			break;
		loc = Parent(loc);
	}
}
template <typename T> T SegmentTree<T>::Eval(size_t start, size_t end) {
	Array<ull> range = GetRange(start, end);
	T counter = Val[range[0]];
	for (ull i = 1; i < range.Length; ++i) {
		counter = add(counter, Val[range[i]]);
	}
	return counter;
}

template <typename T>
ull SegmentTree<T>::CellIndex(size_t start, size_t logsize) {
	return ((1 << (Degree - logsize - 1)) - 1) + (start >> logsize);
}

template <typename T>
Array<ull> SegmentTree<T>::GetRange(size_t start, size_t end) {
	Array<ull> res;
	size_t loc = start;
	for (size_t i = 0; true; ++i) {
		if ((1 << i) + loc > end + 1)
			break;
		if ((1 << i) & loc) {
			res.Push(CellIndex(loc, i));
			loc = (1 << i) + loc;
		}
	}
	if (loc != end + 1) {
		for (size_t i = Log2(SmallerPower2(end + 1 - loc)); true; --i) {
			if ((1 << i) + loc > end + 1)
				continue;
			res.Push(CellIndex(loc, i));
			loc = (1 << i) + loc;
			if (loc == end + 1)
				break;
		}
	}
	return res;
}

template <typename T, typename U> class LazySegmentTree {
	static_assert(std::is_copy_assignable<T>::value);
	static_assert(std::is_copy_assignable<U>::value);

public:
	LazySegmentTree() {}

	template <typename AddFunc, typename Func, typename ConvoluteFunc>
	LazySegmentTree(
	  ull N, T val, AddFunc&& addfunc, Func&& func, ConvoluteFunc&& convolutefunc
	) {
		Init(
		  N, val, std::forward<AddFunc>(addfunc), std::forward<Func>(func),
		  std::forward<ConvoluteFunc>(convolutefunc)
		);
	}

	template <typename AddFunc, typename Func, typename ConvoluteFunc>
	void Init(
	  ull N, T val, AddFunc&& addfunc, Func&& func, ConvoluteFunc&& convolutefunc
	);

	T Eval(ull, ull); // evaluate [a,b]
	void Action(U, ull, ull);
	void Set(ull, T);

	ull Length;
	ull CellLength;
	ull Degree;

	FunctionType<T(T, T)> add;
	FunctionType<T(U, T)> func;
	FunctionType<U(U, U)> convolute; // f,g -> f*g

	LightArray<T> Val;
	LightArray<U> Act;
	LightArray<bool> ActIsNull;
	LightArray<ull> CellStart;
	LightArray<ull> CellEnd;

private:
	void GetRange(ull, ull);
	Stack<ull> ToBeExecuted;
	Stack<ull> ToBeRefreshed;
	Queue<ull> TargetRange;

	void Propagate(ull);
	void Execute(ull);
	void Refresh(ull);
	void ExecuteAndRefreshAll(); // Of ToBeExecuted and ToBeRefreshed
};

template <typename T, typename U>
template <typename AddFunc, typename Func, typename ConvoluteFunc>
void LazySegmentTree<T, U>::Init(
  ull N,
  T init_val,
  AddFunc&& addfunc,
  Func&& funcfunc,
  ConvoluteFunc&& convolutefunc
) {
	add = FunctionType<T(T, T)>(std::forward<AddFunc>(addfunc));
	func = FunctionType<T(U, T)>(std::forward<Func>(funcfunc));
	convolute = FunctionType<U(U, U)>(std::forward<ConvoluteFunc>(convolutefunc));

	Length = BiggerPower2(N);
	CellLength = Length * 2 - 1;
	Degree = Log2(Length * 2);
	Val.Allocate(CellLength);
	Act.Allocate(CellLength);
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
		for (ull i = cell_start; i <= cell_end; ++i) {
			Val[i] = add(Val[Child(i)], Val[Child(i) + 1]);
			CellStart[i] = (i - cell_start) << (Degree - d - 1);
			CellEnd[i] = ((i - cell_start + 1) << (Degree - d - 1)) - 1;
		}
		if (d == 0)
			break;
	}
	return;
}

template <typename T, typename U>
inline void LazySegmentTree<T, U>::GetRange(ull start, ull end) {
	ToBeExecuted.Clear();
	ToBeRefreshed.Clear();
	TargetRange.Clear();

	Queue<size_t> queue;
	queue.Push(0);
	while (queue.Size > 0) {
		size_t loc = queue.Pop();
		if (CellEnd[loc] < start || end < CellStart[loc]) {
			ToBeExecuted.Push(loc);
			continue;
		}

		if (start <= CellStart[loc] && end >= CellEnd[loc]) {
			ToBeExecuted.Push(loc);
			continue;
		}
		Propagate(loc);
		ToBeRefreshed.Push(loc);
		queue.Push(Child(loc));
		queue.Push(Child(loc) + 1);
	}
	Stack<ull> stack;
	stack.Push(0);
	while (stack.Size > 0) {
		ull loc = stack.Pop();
		if (CellEnd[loc] < start || end < CellStart[loc]) {
			continue;
		}

		if (start <= CellStart[loc] && end >= CellEnd[loc]) {
			TargetRange.Push(loc);
			continue;
		}
		stack.Push(Child(loc));
		stack.Push(Child(loc) + 1);
	}
}

template <typename T, typename U>
inline void LazySegmentTree<T, U>::Propagate(ull loc) {
	if (ActIsNull[loc])
		return;
	ActIsNull[loc] = true;
	if (Child(loc) < CellLength) {
		if (!ActIsNull[Child(loc)])
			Act[Child(loc)] = convolute(Act[loc], Act[Child(loc)]);
		else {
			Act[Child(loc)] = Act[loc];
			ActIsNull[Child(loc)] = false;
		}
		if (!ActIsNull[Child(loc) + 1])
			Act[Child(loc) + 1] = convolute(Act[loc], Act[Child(loc) + 1]);
		else {
			Act[Child(loc) + 1] = Act[loc];
			ActIsNull[Child(loc) + 1] = false;
		}
	}
}

template <typename T, typename U>
inline void LazySegmentTree<T, U>::Execute(ull loc) {
	if (ActIsNull[loc])
		return;
	Val[loc] = func(Act[loc], (Val[loc]));
	Propagate(loc);
}

template <typename T, typename U>
inline void LazySegmentTree<T, U>::Refresh(ull loc) {
	if (Child(loc) >= CellLength)
		return;
	Val[loc] = add(Val[Child(loc)], Val[Child(loc) + 1]);
}

template <typename T, typename U>
inline void LazySegmentTree<T, U>::ExecuteAndRefreshAll() {
	while (ToBeExecuted.Size > 0) {
		Execute(ToBeExecuted.Pop());
	}
	while (ToBeRefreshed.Size > 0) {
		Refresh(ToBeRefreshed.Pop());
	}
}

template <typename T, typename U>
inline T LazySegmentTree<T, U>::Eval(ull start, ull end) {
	GetRange(start, end);
	ExecuteAndRefreshAll();
	T counter = Val[TargetRange.Pop()];
	while (TargetRange.Size > 0) {
		counter = add(counter, Val[TargetRange.Pop()]);
	}
	return counter;
}

template <typename T, typename U>
inline void LazySegmentTree<T, U>::Action(U action, ull start, ull end) {
	GetRange(start, end);
	while (TargetRange.Size > 0) {
		ull loc = TargetRange.Pop();
		if (ActIsNull[loc]) {
			Act[loc] = action;
		} else {
			Act[loc] = convolute(action, Act[loc]);
		}
		ActIsNull[loc] = false;
	}
	ExecuteAndRefreshAll();
}

template <typename T, typename U>
inline void LazySegmentTree<T, U>::Set(ull loc, T val) {
	ull loc_cell = Length - 1 + loc;
	GetRange(loc, loc);
	Execute(loc_cell);
	Val[loc_cell] = val;
	ExecuteAndRefreshAll();
}
