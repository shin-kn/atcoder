#include "../standard.cpp"

template <typename T, typename U, typename V> struct LazySegmentAVLTreeCell {
	T ind_min;
	T ind_max;
	U val;
	U sum;
	V act;
	bool act_is_null;
};

template <
  typename T,
  typename U,
  typename V,
  FunctionConcept<U, U, U> AddOp,
  FunctionConcept<U, V, U> FuncOp,
  FunctionConcept<V, V, V> ConvoluteOp,
  typename PropagateOp,
  typename ExecuteOp,
  typename RefreshOp,
  typename BeforeChangeOp,
  typename AfterChangeOp>
// T: index, U: value, V:action
class LazySegmentAVLTreeClass {
public:
	LazySegmentAVLTreeClass(
	  TypeVar<T>,
	  TypeVar<U>,
	  TypeVar<V>,
	  const AddOp& add,
	  const FuncOp& func,
	  const ConvoluteOp& convolute,
	  const PropagateOp& propagate,
	  const ExecuteOp& execute,
	  const RefreshOp& refresh,
	  const BeforeChangeOp& before_change,
	  const AfterChangeOp& afterchange
	);

	T Index() { return avltree.Index(); }

	// index and value of the ind-th element in index order
	Pair<T, U&> Nth(ull ind) {
		auto cell = avltree.Nth(ind);
		return Pair<T, U&>(cell.val1, cell.val2.val);
	}

	void Push(T, U);
	bool Find(T);
	bool Leq(T);
	bool Geq(T);
	void Delete();

	U Eval(T, T);
	void Act(V, T, T);

	using Cell = LazySegmentAVLTreeCell<T, U, V>;

	using AVLTree =
	  AVLTree<T, Cell, decltype(LTOp<T>), BeforeChangeOp, AfterChangeOp>;

	using Node = AVLTreeNode<T, Cell>;

	AddOp add;
	FuncOp func;
	ConvoluteOp convolute; // f,g -> f*g

	PropagateOp propagate;
	ExecuteOp execute;
	RefreshOp refresh;

	BeforeChangeOp before_change;
	AfterChangeOp after_change;

	AVLTree avltree;
};

template <
  typename T,
  typename U,
  typename V,
  FunctionConcept<U, U, U> AddOp,
  FunctionConcept<U, V, U> FuncOp,
  FunctionConcept<V, V, V> ConvoluteOp,
  typename PropagateOp,
  typename ExecuteOp,
  typename RefreshOp,
  typename BeforeChangeOp,
  typename AfterChangeOp>
LazySegmentAVLTreeClass<
  T,
  U,
  V,
  AddOp,
  FuncOp,
  ConvoluteOp,
  PropagateOp,
  ExecuteOp,
  RefreshOp,
  BeforeChangeOp,
  AfterChangeOp>::
  LazySegmentAVLTreeClass(
    TypeVar<T>,
    TypeVar<U>,
    TypeVar<V>,
    const AddOp& add,
    const FuncOp& func,
    const ConvoluteOp& convolute,
    const PropagateOp& propagate,
    const ExecuteOp& execute,
    const RefreshOp& refresh,
    const BeforeChangeOp& before_change,
    const AfterChangeOp& after_change
  )
    : add(add), func(func), convolute(convolute), propagate(propagate),
      execute(execute), refresh(refresh), before_change(before_change),
      after_change(after_change),
      avltree(
        AVLTree(Type<T>, Type<Cell>, LTOp<T>, before_change, after_change, true)
      ) {}

template <
  typename T,
  typename U,
  typename V,
  FunctionConcept<U, U, U> AddOp,
  FunctionConcept<U, V, U> FuncOp,
  FunctionConcept<V, V, V> ConvoluteOp,
  typename PropagateOp,
  typename ExecuteOp,
  typename RefreshOp,
  typename BeforeChangeOp,
  typename AfterChangeOp>
U LazySegmentAVLTreeClass<
  T,
  U,
  V,
  AddOp,
  FuncOp,
  ConvoluteOp,
  PropagateOp,
  ExecuteOp,
  RefreshOp,
  BeforeChangeOp,
  AfterChangeOp>::Eval(T start, T end) {
	if (avltree.Root == nullptr)
		return U();
	U counter = U();
	auto dfs = [&](auto&& self, Node* node) -> void {
		if (node == nullptr)
			return;
		Cell& cell = node->val;
		if (cell.ind_max < start || end < cell.ind_min) { // out of range
			// read only: no parent is refreshed, so its pending act can stay
			return;
		}
		if (start <= cell.ind_min && end >= cell.ind_max) { // in the range
			execute(node);
			counter = add(counter, cell.sum);
			return;
		}
		// else
		// execute (not just propagate) applies act to sum and val directly, so no
		// refresh is needed. Relies on func(a, add(x, y)) == add(func(a, x),
		// func(a, y))
		execute(node);
		self(self, node->child1);
		if (start <= node->index && end >= node->index) {
			counter = add(counter, cell.val);
		}
		self(self, node->child2);
	};
	dfs(dfs, avltree.Root);
	return counter;
}

template <
  typename T,
  typename U,
  typename V,
  FunctionConcept<U, U, U> AddOp,
  FunctionConcept<U, V, U> FuncOp,
  FunctionConcept<V, V, V> ConvoluteOp,
  typename PropagateOp,
  typename ExecuteOp,
  typename RefreshOp,
  typename BeforeChangeOp,
  typename AfterChangeOp>
inline void LazySegmentAVLTreeClass<
  T,
  U,
  V,
  AddOp,
  FuncOp,
  ConvoluteOp,
  PropagateOp,
  ExecuteOp,
  RefreshOp,
  BeforeChangeOp,
  AfterChangeOp>::Act(V action, T start, T end) {
	if (avltree.Root == nullptr)
		return;
	auto dfs = [&](auto&& self, Node* node) -> void {
		if (node == nullptr)
			return;
		Cell& cell = node->val;
		if (cell.ind_max < start || end < cell.ind_min) { // out of range
			execute(node);
			return;
		}
		if (start <= cell.ind_min && end >= cell.ind_max) { // in the range
			if (cell.act_is_null) {
				cell.act = action;
			} else {
				cell.act = convolute(action, cell.act);
			}
			cell.act_is_null = false;
			execute(node);
			return;
		}
		// else
		propagate(node);
		self(self, node->child1);
		if (start <= node->index && end >= node->index) {
			cell.val = func(action, cell.val);
		}
		self(self, node->child2);
		refresh(node);
	};
	dfs(dfs, avltree.Root);
}

template <
  typename T,
  typename U,
  typename V,
  FunctionConcept<U, U, U> AddOp,
  FunctionConcept<U, V, U> FuncOp,
  FunctionConcept<V, V, V> ConvoluteOp,
  typename PropagateOp,
  typename ExecuteOp,
  typename RefreshOp,
  typename BeforeChangeOp,
  typename AfterChangeOp>
void LazySegmentAVLTreeClass<
  T,
  U,
  V,
  AddOp,
  FuncOp,
  ConvoluteOp,
  PropagateOp,
  ExecuteOp,
  RefreshOp,
  BeforeChangeOp,
  AfterChangeOp>::Push(T ind, U val) {
	Cell cell;
	cell.val = val;
	cell.act_is_null = true;

	avltree.Push(ind, cell);
}

template <
  typename T,
  typename U,
  typename V,
  FunctionConcept<U, U, U> AddOp,
  FunctionConcept<U, V, U> FuncOp,
  FunctionConcept<V, V, V> ConvoluteOp,
  typename PropagateOp,
  typename ExecuteOp,
  typename RefreshOp,
  typename BeforeChangeOp,
  typename AfterChangeOp>
bool LazySegmentAVLTreeClass<
  T,
  U,
  V,
  AddOp,
  FuncOp,
  ConvoluteOp,
  PropagateOp,
  ExecuteOp,
  RefreshOp,
  BeforeChangeOp,
  AfterChangeOp>::Find(T ind) {
	return avltree.Find(ind);
}

template <
  typename T,
  typename U,
  typename V,
  FunctionConcept<U, U, U> AddOp,
  FunctionConcept<U, V, U> FuncOp,
  FunctionConcept<V, V, V> ConvoluteOp,
  typename PropagateOp,
  typename ExecuteOp,
  typename RefreshOp,
  typename BeforeChangeOp,
  typename AfterChangeOp>
bool LazySegmentAVLTreeClass<
  T,
  U,
  V,
  AddOp,
  FuncOp,
  ConvoluteOp,
  PropagateOp,
  ExecuteOp,
  RefreshOp,
  BeforeChangeOp,
  AfterChangeOp>::Leq(T ind) {
	return avltree.Leq(ind);
}

template <
  typename T,
  typename U,
  typename V,
  FunctionConcept<U, U, U> AddOp,
  FunctionConcept<U, V, U> FuncOp,
  FunctionConcept<V, V, V> ConvoluteOp,
  typename PropagateOp,
  typename ExecuteOp,
  typename RefreshOp,
  typename BeforeChangeOp,
  typename AfterChangeOp>
bool LazySegmentAVLTreeClass<
  T,
  U,
  V,
  AddOp,
  FuncOp,
  ConvoluteOp,
  PropagateOp,
  ExecuteOp,
  RefreshOp,
  BeforeChangeOp,
  AfterChangeOp>::Geq(T ind) {
	return avltree.Geq(ind);
}

template <
  typename T,
  typename U,
  typename V,
  FunctionConcept<U, U, U> AddOp,
  FunctionConcept<U, V, U> FuncOp,
  FunctionConcept<V, V, V> ConvoluteOp,
  typename PropagateOp,
  typename ExecuteOp,
  typename RefreshOp,
  typename BeforeChangeOp,
  typename AfterChangeOp>
void LazySegmentAVLTreeClass<
  T,
  U,
  V,
  AddOp,
  FuncOp,
  ConvoluteOp,
  PropagateOp,
  ExecuteOp,
  RefreshOp,
  BeforeChangeOp,
  AfterChangeOp>::Delete() {
	avltree.Delete();
}

template <
  typename T,
  typename U,
  typename V,
  FunctionConcept<U, U, U> AddOp,
  FunctionConcept<U, V, U> FuncOp,
  FunctionConcept<V, V, V> ConvoluteOp>
auto LazySegmentAVLTree(
  TypeVar<T>,
  TypeVar<U>,
  TypeVar<V>,
  const AddOp& add,
  const FuncOp& func,
  const ConvoluteOp& convolute
) {

	using Cell = LazySegmentAVLTreeCell<T, U, V>;
	using Node = AVLTreeNode<T, Cell>;
	auto propagate = [=](Node* node) {
		if (node == nullptr)
			return;

		Cell& cell = node->val;
		if (cell.act_is_null)
			return;
		cell.act_is_null = true;
		cell.val = func(cell.act, cell.val);
		if (node->child1 != nullptr) {
			Cell& cell1 = node->child1->val;
			if (cell1.act_is_null)
				cell1.act = cell.act;
			else
				cell1.act = convolute(cell.act, cell1.act);
			cell1.act_is_null = false;
		}
		if (node->child2 != nullptr) {
			Cell& cell2 = node->child2->val;
			if (cell2.act_is_null)
				cell2.act = cell.act;
			else
				cell2.act = convolute(cell.act, cell2.act);
			cell2.act_is_null = false;
		}
	};

	auto execute = [=](Node* node) {
		if (node == nullptr)
			return;
		Cell& cell = node->val;
		if (cell.act_is_null)
			return;
		cell.sum = func(cell.act, cell.sum);
		propagate(node);
	};

	auto refresh = [=](Node* node) {
		Cell& cell = node->val;
		cell.sum = cell.val;
		if (node->child1 != nullptr) {
			cell.sum = add(node->child1->val.sum, cell.sum);
		}
		if (node->child2 != nullptr) {
			cell.sum = add(cell.sum, node->child2->val.sum);
		}
		cell.ind_min = node->index;
		cell.ind_max = node->index;
		if (node->child1 != nullptr)
			cell.ind_min = node->child1->val.ind_min;
		if (node->child2 != nullptr)
			cell.ind_max = node->child2->val.ind_max;
	};

	decltype(propagate) before_change(propagate);

	auto after_change = [=](Node* node) {
		if (node == nullptr)
			return;
		execute(node->child1);
		execute(node->child2);
		refresh(node);
	};

	LazySegmentAVLTreeClass<
	  T, U, V, AddOp, FuncOp, ConvoluteOp, decltype(propagate), decltype(execute),
	  decltype(refresh), decltype(before_change), decltype(after_change)>
	  segtree(
	    Type<T>, Type<U>, Type<V>, add, func, convolute, propagate, execute,
	    refresh, before_change, after_change
	  );
	return segtree;
}
