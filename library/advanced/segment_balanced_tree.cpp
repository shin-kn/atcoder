#include "../standard.cpp"

template <typename T, typename U, typename V>
// T: index, U: value, V:action
class LazySegmentAVLTree {
public:
	LazySegmentAVLTree() {}

	template <typename... Args> LazySegmentAVLTree(Args&&... args) {
		Init(std::forward<Args>(args)...);
	}

	template <typename AddFunc, typename Func, typename ConvoluteFunc>
	void Init(AddFunc&& addfunc, Func&& func, ConvoluteFunc&& convolutefunc);

	T Index() { return avltree.Index(); }

	void Push(T, U);
	bool Find(T);
	bool Leq(T);
	bool Geq(T);
	void Delete();

	U Eval(T, T);
	void Action(V, T, T);

	void GetRange(T, T);

	struct Cell {
		T ind_min;
		T ind_max;
		U val;
		U sum;
		V act;
		bool act_is_null;
	};

	AVLTree<T, Cell, true> avltree;

	using Node = AVLTree<T, Cell, true>::Node;

	FunctionType<U(U, U)> add;
	FunctionType<U(V, U)> func;
	FunctionType<V(V, V)> convolute; // f,g -> f*g

	FunctionType<void(Node*)> propagate;
	FunctionType<void(Node*)> execute;
	FunctionType<void(Node*)> refresh;
	void ExecuteAndRefreshAll();

	Stack<Node*> ToBeExecuted;  //
	Stack<Node*> ToBeRefreshed; // Nodes
	Queue<Pair<U*, Node*>> TargetRange;
};

template <typename T, typename U, typename V>
void LazySegmentAVLTree<T, U, V>::GetRange(T start, T end) {
	auto bfs_runner = [&](Node* node) -> bool {
		Cell& cell = node->val;
		if (cell.ind_max < start || end < cell.ind_min) {
			ToBeExecuted.Push(node);
		}
		if (start <= cell.ind_min && end >= cell.ind_max) {
			ToBeExecuted.Push(node);
			return false;
		}
		propagate(node);
		ToBeRefreshed.Push(node);
		return true;
	};

	ToBeExecuted.Clear();
	ToBeRefreshed.Clear();
	TargetRange.Clear();

	avltree.BFS(bfs_runner);

	auto dfs_func = [&](Node* node) {};
	auto dfs_canreach = [&](Node* node) -> bool {
		Cell& cell = node->val;
		if (cell.ind_max < start || end < cell.ind_min) {
			return false;
		}
		if (start <= cell.ind_min && end >= cell.ind_max) {
			TargetRange.Push(Pair<U*, Node*>(&(cell.sum), node));
			return false;
		}
		if (start <= node->index && end >= node->index) {
			TargetRange.Push(Pair<U*, Node*>(&(cell.val), nullptr));
		}
		return true;
	};
	avltree.DFS(dfs_func, dfs_canreach);
}

template <typename T, typename U, typename V>
void LazySegmentAVLTree<T, U, V>::ExecuteAndRefreshAll() {
	while (ToBeExecuted.Size > 0) {
		execute(ToBeExecuted.Pop());
	}
	while (ToBeRefreshed.Size > 0) {
		refresh(ToBeRefreshed.Pop());
	}
}

template <typename T, typename U, typename V>
U LazySegmentAVLTree<T, U, V>::Eval(T start, T end) {
	GetRange(start, end);
	ExecuteAndRefreshAll();
	if (TargetRange.Size == 0)
		return U();
	U counter = *(TargetRange.Pop().val1);
	while (TargetRange.Size > 0) {
		counter = add(counter, *(TargetRange.Pop().val1));
	}
	return counter;
}

template <typename T, typename U, typename V>
inline void LazySegmentAVLTree<T, U, V>::Action(V action, T start, T end) {
	GetRange(start, end);
	while (TargetRange.Size > 0) {
		Pair<U*, Node*> loc = TargetRange.Pop();
		if (loc.val2 == nullptr) {
			*(loc.val1) = func(action, *(loc.val1));
		} else {
			Cell& cell = loc.val2->val;
			if (cell.act_is_null) {
				cell.act = action;
			} else {
				cell.act = convolute(action, cell.act);
			}
			cell.act_is_null = false;
		}
	}
	ExecuteAndRefreshAll();
}

template <typename T, typename U, typename V>
void LazySegmentAVLTree<T, U, V>::Push(T ind, U val) {
	Cell cell;
	cell.val = val;
	cell.act_is_null = true;

	avltree.Push(ind, cell);
}

template <typename T, typename U, typename V>
bool LazySegmentAVLTree<T, U, V>::Find(T ind) {
	return avltree.Find(ind);
}

template <typename T, typename U, typename V>
bool LazySegmentAVLTree<T, U, V>::Leq(T ind) {
	return avltree.Leq(ind);
}

template <typename T, typename U, typename V>
bool LazySegmentAVLTree<T, U, V>::Geq(T ind) {
	return avltree.Geq(ind);
}

template <typename T, typename U, typename V>
void LazySegmentAVLTree<T, U, V>::Delete() {
	avltree.Delete();
}

template <typename T, typename U, typename V>
template <typename AddFunc, typename Func, typename ConvoluteFunc>
void LazySegmentAVLTree<T, U, V>::Init(
AddFunc&& addfunc, Func&& funcfunc, ConvoluteFunc&& convolutefunc) {
	add = FunctionType<U(U, U)>(std::forward<AddFunc>(addfunc));
	func = FunctionType<U(V, U)>(std::forward<Func>(funcfunc));
	convolute = FunctionType<V(V, V)>(std::forward<ConvoluteFunc>(convolutefunc));

	auto propagate_lambda = [&](Node* node) {
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

	propagate = FunctionType<void(Node*)>(propagate_lambda);

	auto execute_lambda = [&](Node* node) {
		if (node == nullptr)
			return;
		Cell& cell = node->val;
		if (cell.act_is_null)
			return;
		cell.sum = func(cell.act, cell.sum);
		propagate(node);
	};

	execute = FunctionType<void(Node*)>(execute_lambda);

	auto refresh_lambda = [&](Node* node) {
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

	refresh = FunctionType<void(Node*)>(refresh_lambda);

	avltree.before_change = FunctionType<void(Node*)>(propagate);

	auto after_change_lambda = [&](Node* node) {
		if (node == nullptr)
			return;
		execute(node->child1);
		execute(node->child2);
		refresh(node);
	};
	avltree.after_change = FunctionType<void(Node*)>(after_change_lambda);
}
