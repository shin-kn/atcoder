#include "../standard.cpp"

template <typename T, ull N> class MultiDimSegmentTree {
	static_assert(std::is_copy_assignable<T>::value);

public:
	MultiDimSegmentTree() {}
	template <typename... Args> MultiDimSegmentTree(Args&&... args) {
		Init(std::forward<Args>(args)...);
	}

	template <FunctionConcept<T, T, T> AddFunc>
	void Init(::Set<ull, N>, T, AddFunc&&);

	FunctionType<T(T, T)> add;

	ull Length;
	ull CellLength;
	ull Degree;

	LightArray<MultiDimSegmentTree<T, N - 1>> Val;

	T Eval(::Set<ull, N>, ::Set<ull, N>);
	Stack<ull> eval_stack;
	void Set(::Set<ull, N> loc, T val) {
		set(loc, val);
		del_dif();
	}
	void set(::Set<ull, N>, T);
	void del_dif();

	void update_dif(MultiDimSegmentTree&, MultiDimSegmentTree&);
	LightArray<ull> dif_array;
	bool has_dif = false;
	void update_full(MultiDimSegmentTree&, MultiDimSegmentTree&);

	ull CellIndex(size_t start, size_t logsize);
};

template <typename T, ull N>
template <FunctionConcept<T, T, T> AddFunc>
void MultiDimSegmentTree<T, N>::Init(
  ::Set<ull, N> size, T init_val, AddFunc&& func
) {
	add = FunctionType<T(T, T)>(std::forward<AddFunc>(func));

	Length = BiggerPower2(size[0]);
	CellLength = Length * 2 - 1;
	Degree = Log2(Length * 2);
	dif_array.Allocate(Degree);

	Val.Allocate(CellLength);
	::Set<ull, N - 1> next_size;

	for (ull i = 0; i < N - 1; ++i)
		next_size[i] = size[i + 1];
	for (ull i = 0; i < CellLength; ++i) {
		Val[i].Init(next_size, init_val, std::forward<AddFunc>(func));
	}
	if (Degree == 1)
		return;
	for (ull d = Degree - 2;; --d) {
		ull cell_start = (1ull << (d)) - 1;
		ull cell_end = (1ull << (d + 1)) - 2;
		for (ull i = cell_start; i <= cell_end; ++i) {
			Val[i].update_full(Val[Child(i)], Val[Child(i) + 1]);
		}
		if (d == 0)
			break;
	}
	return;
}

template <typename T, ull N>
void MultiDimSegmentTree<T, N>::update_full(
  MultiDimSegmentTree& tree1, MultiDimSegmentTree& tree2
) {
	for (ull i = 0; i < CellLength; ++i) {
		Val[i].update_full(tree1.Val[i], tree2.Val[i]);
	}
}

template <typename T, ull N>
void MultiDimSegmentTree<T, N>::update_dif(
  MultiDimSegmentTree& tree1, MultiDimSegmentTree& tree2
) {
	LightArray<ull>& dif(tree1.has_dif ? tree1.dif_array : tree2.dif_array);
	for (ull i = 0; i < Degree; ++i) {
		ull loc = dif[i];
		Val[loc].update_dif(tree1.Val[loc], tree2.Val[loc]);
		dif_array[i] = loc;
	}
	has_dif = true;
}

template <typename T, ull N>
void MultiDimSegmentTree<T, N>::set(::Set<ull, N> loc, T val) {
	ull cell_loc = Length - 1 + loc[0];
	::Set<ull, N - 1> new_loc;
	for (ull i = 0; i < N - 1; ++i)
		new_loc[i] = loc[i + 1];
	Val[cell_loc].set(new_loc, val);
	dif_array[0] = cell_loc;
	for (ull i = 1; i < Degree; ++i) {
		cell_loc = Parent(cell_loc);
		Val[cell_loc].update_dif(Val[Child(cell_loc)], Val[Child(cell_loc) + 1]);
		dif_array[i] = cell_loc;
	}
	has_dif = true;
}

template <typename T, ull N> void MultiDimSegmentTree<T, N>::del_dif() {
	for (ull i = 0; i < Degree; ++i) {
		Val[dif_array[i]].del_dif();
	}
	has_dif = false;
}

template <typename T, ull N>
ull MultiDimSegmentTree<T, N>::CellIndex(size_t start, size_t logsize) {
	return ((1 << (Degree - logsize - 1)) - 1) + (start >> logsize);
}

template <typename T, ull N>
T MultiDimSegmentTree<T, N>::Eval(
  ::Set<ull, N> start_set, ::Set<ull, N> end_set
) {
	ull start = start_set[0];
	ull end = end_set[0];

	ull loc = start;
	for (size_t i = 0; true; ++i) {
		if ((1 << i) + loc > end + 1)
			break;
		if ((1 << i) & loc) {
			eval_stack.Push(CellIndex(loc, i));
			loc = (1 << i) + loc;
		}
	}
	if (loc != end + 1) {
		for (size_t i = Log2(SmallerPower2(end + 1 - loc)); true; --i) {
			if ((1 << i) + loc > end + 1)
				continue;
			eval_stack.Push(CellIndex(loc, i));
			loc = (1 << i) + loc;
			if (loc == end + 1)
				break;
		}
	}

	::Set<ull, N - 1> new_start_set;
	::Set<ull, N - 1> new_end_set;
	for (ull i = 0; i < N - 1; ++i) {
		new_start_set[i] = start_set[i + 1];
		new_end_set[i] = end_set[i + 1];
	}

	T counter;
	counter = Val[eval_stack.Pop()].Eval(new_start_set, new_end_set);
	while (eval_stack.Size > 0) {
		counter =
		  add(counter, Val[eval_stack.Pop()].Eval(new_start_set, new_end_set));
	}
	return counter;
}

// for primitive case
template <typename T> class MultiDimSegmentTree<T, 0> {
public:
	MultiDimSegmentTree() {}
	template <typename... Args> MultiDimSegmentTree(Args&&... args) {
		Init(std::forward<Args>(args)...);
	}

	template <FunctionConcept<T, T, T> AddFunc>
	void Init(::Set<ull, 0>, T init_val, AddFunc&& func) {
		add = FunctionType<T(T, T)>(std::forward<AddFunc>(func));
		val = init_val;
	}

	FunctionType<T(T, T)> add;

	T val;
	T Eval(::Set<ull, 0>, ::Set<ull, 0>) { return val; }

	void set(::Set<ull, 0>, T set_val) { val = set_val; }
	void del_dif() {}

	void update_dif(MultiDimSegmentTree& val1, MultiDimSegmentTree& val2) {
		val = add(val1.val, val2.val);
	}
	void update_full(MultiDimSegmentTree& val1, MultiDimSegmentTree& val2) {
		val = add(val1.val, val2.val);
	}
};
