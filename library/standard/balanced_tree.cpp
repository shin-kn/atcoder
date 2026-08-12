#pragma once
#include "../basic.cpp"
template <typename T, typename U> struct AVLTreeNode {
	AVLTreeNode* child1;
	AVLTreeNode* child2;
	ull length; // used for balancing, >=1
	ull weight; // used for an array like feature, >=1
	T index;
	U val;
	void Clear();
};

template <typename T, typename U> void AVLTreeNode<T, U>::Clear() {
	child1 = nullptr;
	child2 = nullptr;
	weight = 1;
	length = 1;
}

template <
  typename T,
  typename U,
  FunctionConcept<bool, T, T> LessFunc = decltype(LTOp<T>),
  typename BeforeChangeFunc = decltype(DoNothingFunc<void, AVLTreeNode<T, U>*>),
  typename AfterChangeFunc = decltype(DoNothingFunc<void, AVLTreeNode<T, U>*>)>
class AVLTree {
public:
	using Node = AVLTreeNode<T, U>;
	AVLTree(
	  TypeVar<T>,
	  TypeVar<U>,
	  LessFunc& less = LTOp<T>,
	  BeforeChangeFunc& before_change = DoNothingFunc<void, Node*>,
	  AfterChangeFunc& after_change = DoNothingFunc<void, Node*>,
	  bool WithExtension = false
	)
	    : less(less), before_change(before_change), after_change(after_change),
	      WithExtension(WithExtension) {}

	ull Size = 0;

	void Push(T, U);

	bool Find(T);
	bool Leq(T);
	bool Geq(T);
	ull Number() { return CurrentNumber; }

	void Delete();
	T Index() { return Current->index; }
	T Index(T ind) {
		Find(ind);
		return Index();
	}
	U& Data() { return Current->val; }
	U& Data(T ind) {
		Find(ind);
		return Data();
	}

	Pair<T, U&> operator[](ull ind); // equally, Nth

	::Array<T> IndexArray();
	::Array<U> DataArray();
	::Array<Pair<T, U>> Array();

	// internal things

	LessFunc less;
	// For Extension
	BeforeChangeFunc before_change;
	AfterChangeFunc after_change;

	bool WithExtension;

	::Array<Node> Nodes;

	Node* Balance(Node*);
	void BalanceRoutes(Node*);
	// use FindRoutes

	Node* LL_Rotate(Node*);
	Node* LR_Rotate(Node*);
	Node* RL_Rotate(Node*);
	Node* RR_Rotate(Node*);

	void Update(Node*);

	Stack<Pair<Node*, bool>> FindRoutes;

	// return top of the node
	// assume childrens of the node is balanced, and their weight and length are
	// set correctly

	Stack<Node*> AvailNodes;
	Node* Root = nullptr;
	Node* Current;
	ull CurrentNumber;

	void DFS(FunctionType<void(Node*)>, FunctionType<bool(Node*)>);
	// by the order of DFS, execute func for Node*

	void BFS(FunctionType<bool(Node*)>);
	// by the order of BFS, execute func for Node*

	template <typename F> void DFS(F&& func) {
		DFS(std::forward<F>(func), [&](Node*) -> bool { return true; });
	}

	::Array<Node*, true> dfs_loc;
	::Array<ull, true> dfs_num;

	Queue<Node*> bfs_queue;

	// utils

	inline ull length(Node* node) { return node == nullptr ? 0 : node->length; }
	inline ull weight(Node* node) { return node == nullptr ? 0 : node->weight; }
	inline bool leq(T ind1, T ind2) { return !less(ind2, ind1); }
	inline bool geq(T ind1, T ind2) { return !less(ind1, ind2); }
};

template <
  typename T,
  typename U,
  FunctionConcept<bool, T, T> LessFunc,
  typename BeforeChangeFunc,
  typename AfterChangeFunc>
Pair<T, U&>
AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::operator[](
  ull index
) {
	assert(index < Size);
	Node* loc = Root;
	ull counter = 0;
	while (true) {
		ull thisindex = weight(loc->child1) + counter;
		if (thisindex == index)
			break;
		if (thisindex < index) {
			counter += weight(loc->child1) + 1;
			loc = loc->child2;
			continue;
		}
		loc = loc->child1;
	}
	Current = loc;
	CurrentNumber = index;
	return Pair<T, U&>(loc->index, loc->val);
}

template <
  typename T,
  typename U,
  FunctionConcept<bool, T, T> LessFunc,
  typename BeforeChangeFunc,
  typename AfterChangeFunc>
::Array<Pair<T, U>>
AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::Array() {
	::Array<Pair<T, U>> arr;
	arr.Allocate(Size);
	auto append = [&](Node* node) {
		arr[arr.Length] = Pair<T, U>(node->index, node->val);
	};
	DFS(append);
	return arr;
}

template <
  typename T,
  typename U,
  FunctionConcept<bool, T, T> LessFunc,
  typename BeforeChangeFunc,
  typename AfterChangeFunc>
::Array<U>
AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::DataArray() {
	::Array<U> arr;
	arr.Allocate(Size);
	auto append = [&](Node* node) { arr[arr.Length] = node->val; };
	DFS(append);
	return arr;
}

template <
  typename T,
  typename U,
  FunctionConcept<bool, T, T> LessFunc,
  typename BeforeChangeFunc,
  typename AfterChangeFunc>
::Array<T>
AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::IndexArray() {
	::Array<T> arr;
	arr.Allocate(Size);
	auto append = [&](Node* node) { arr[arr.Length] = node->index; };
	DFS(append);
	return arr;
}

template <
  typename T,
  typename U,
  FunctionConcept<bool, T, T> LessFunc,
  typename BeforeChangeFunc,
  typename AfterChangeFunc>
void AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::BFS(
  FunctionType<bool(Node*)> func
) {
	if (Root == nullptr)
		return;
	bfs_queue.Clear();
	bfs_queue.Push(Root);
	while (bfs_queue.Size > 0) {
		Node* node = bfs_queue.Pop();
		if (!func(node))
			continue;
		if (node->child1 != nullptr)
			bfs_queue.Push(node->child1);
		if (node->child2 != nullptr)
			bfs_queue.Push(node->child2);
	}
}

template <
  typename T,
  typename U,
  FunctionConcept<bool, T, T> LessFunc,
  typename BeforeChangeFunc,
  typename AfterChangeFunc>
void AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::DFS(
  FunctionType<void(Node*)> func, FunctionType<bool(Node*)> canreach
) {
	if (!canreach(Root))
		return;
	if (Size == 0)
		return;
	dfs_loc.Length = 0;
	dfs_num.Length = 0;
	ull place = 0;
	dfs_loc(0) = Root;
	dfs_num(0) = 0;
	while (true) {
		if (dfs_num[place] == 2) {
			if (place == 0)
				break;
			--place;
			++dfs_num[place];
			continue;
		}
		if (dfs_num[place] == 1) {
			func(dfs_loc[place]);
		}
		Node* togo =
		  dfs_num[place] == 0 ? dfs_loc[place]->child1 : dfs_loc[place]->child2;
		if (togo == nullptr || !canreach(togo)) {
			++dfs_num[place];
			continue;
		}
		++place;
		dfs_loc(place) = togo;
		dfs_num(place) = 0;
	}
}

template <
  typename T,
  typename U,
  FunctionConcept<bool, T, T> LessFunc,
  typename BeforeChangeFunc,
  typename AfterChangeFunc>
void AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::Push(
  T index, U val
) {
	++Size;
	Node* newnode = nullptr;
	if (AvailNodes.Size > 0) {
		newnode = AvailNodes.Pop();
	} else {
		newnode = &Nodes[Nodes.Length];
	}
	newnode->Clear();
	newnode->index = index;
	newnode->val = val;

	if (WithExtension) {
		after_change(newnode);
	}

	Current = newnode;

	FindRoutes.Clear();

	if (Root == nullptr) {
		BalanceRoutes(newnode);
		return;
	}

	Node* loc = Root;
	while (true) {
		before_change(loc);
		if (less(index, loc->index)) {
			if (loc->child1 == nullptr) {
				loc->child1 = newnode;
				break;
			} else {
				FindRoutes.Push(Pair<Node*, bool>(loc, true));
				loc = loc->child1;
			}
		} else {
			if (loc->child2 == nullptr) {
				loc->child2 = newnode;
				break;
			} else {
				FindRoutes.Push(Pair<Node*, bool>(loc, false));
				loc = loc->child2;
			}
		}
	}
	BalanceRoutes(loc);
}

template <
  typename T,
  typename U,
  FunctionConcept<bool, T, T> LessFunc,
  typename BeforeChangeFunc,
  typename AfterChangeFunc>
bool AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::Leq(T index) {
	if (Root == nullptr)
		return false;
	Node* BestCurrent = nullptr;
	ull BestCurrentNumber = 0;
	Current = Root;
	CurrentNumber = 0;
	FindRoutes.Clear();
	while (true) {
		if (leq(Current->index, index)) {
			BestCurrent = Current;
			BestCurrentNumber = CurrentNumber + weight(Current->child1);
			FindRoutes.Push(Pair<Node*, bool>(Current, false));
			CurrentNumber += weight(Current->child1) + 1;
			if (Current->child2 == nullptr)
				break;
			Current = Current->child2;
		} else {
			FindRoutes.Push(Pair<Node*, bool>(Current, true));
			if (Current->child1 == nullptr)
				break;
			Current = Current->child1;
		}
	}
	if (BestCurrent == nullptr)
		return false;
	if (BestCurrent != Current) {
		while (true) {
			if (FindRoutes.Pop().val1 == BestCurrent)
				break;
		}
	}
	Current = BestCurrent;
	CurrentNumber = BestCurrentNumber;
	return true;
}

template <
  typename T,
  typename U,
  FunctionConcept<bool, T, T> LessFunc,
  typename BeforeChangeFunc,
  typename AfterChangeFunc>
bool AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::Geq(T index) {
	if (Root == nullptr)
		return false;
	Node* BestCurrent = nullptr;
	ull BestCurrentNumber = 0;
	Current = Root;
	CurrentNumber = 0;
	FindRoutes.Clear();
	while (true) {
		if (geq(Current->index, index)) {
			BestCurrent = Current;
			BestCurrentNumber = CurrentNumber + weight(Current->child1);
			FindRoutes.Push(Pair<Node*, bool>(Current, true));
			if (Current->child1 == nullptr)
				break;
			Current = Current->child1;
		} else {
			FindRoutes.Push(Pair<Node*, bool>(Current, false));
			if (Current->child2 == nullptr)
				break;
			CurrentNumber += weight(Current->child1) + 1;
			Current = Current->child2;
		}
	}
	if (BestCurrent == nullptr)
		return false;
	if (BestCurrent != Current) {
		while (true) {
			if (FindRoutes.Pop().val1 == BestCurrent)
				break;
		}
	}
	Current = BestCurrent;
	CurrentNumber = BestCurrentNumber;
	return true;
}

template <
  typename T,
  typename U,
  FunctionConcept<bool, T, T> LessFunc,
  typename BeforeChangeFunc,
  typename AfterChangeFunc>
bool AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::Find(T index) {
	if (Root == nullptr)
		return false;
	Current = Root;
	CurrentNumber = 0;
	FindRoutes.Clear();
	while (Current != nullptr) {
		if (less(index, Current->index)) {
			FindRoutes.Push(Pair<Node*, bool>(Current, true));
			Current = Current->child1;
			continue;
		}
		if (less(Current->index, index)) {
			FindRoutes.Push(Pair<Node*, bool>(Current, false));
			CurrentNumber += weight(Current->child1) + 1;
			Current = Current->child2;
			continue;
		}
		CurrentNumber += weight(Current->child1);
		break;
	}
	if (Current == nullptr) {
		return false;
	}
	return true;
}

template <
  typename T,
  typename U,
  FunctionConcept<bool, T, T> LessFunc,
  typename BeforeChangeFunc,
  typename AfterChangeFunc>
void AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::Delete() {
	if (Current == nullptr)
		return;
	--Size;
	AvailNodes.Push(Current);

	if (WithExtension)
		before_change(Current);

	if (Current->child1 == nullptr) {
		BalanceRoutes(Current->child2);
		return;
	}

	if (Current->child1->child2 == nullptr) {
		if (WithExtension)
			before_change(Current->child1);

		Current->child1->child2 = Current->child2;
		BalanceRoutes(Current->child1);
		return;
	}

	Node* loc = Current->child1;
	while (loc->child2 != nullptr) {
		if (WithExtension)
			before_change(loc);

		loc = loc->child2;
	}
	Node* n_current = loc;
	if (WithExtension)
		before_change(n_current);
	n_current->child2 = Current->child2;

	FindRoutes.Push(Pair<Node*, bool>(n_current, true));
	loc = Current->child1;
	while (loc->child2 != n_current) {
		FindRoutes.Push(Pair<Node*, bool>(loc, false));
		loc = loc->child2;
	}
	loc->child2 = n_current->child1;
	BalanceRoutes(loc);
}

template <
  typename T,
  typename U,
  FunctionConcept<bool, T, T> LessFunc,
  typename BeforeChangeFunc,
  typename AfterChangeFunc>
void AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::BalanceRoutes(
  AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::Node* loc
) {
	loc = Balance(loc);

	while (FindRoutes.Size > 0) {
		Pair<Node*, bool> route = FindRoutes.Pop();
		Node* thisnode = route.val1;
		(route.val2 ? thisnode->child1 : thisnode->child2) = loc;
		loc = Balance(thisnode);
	}

	Root = loc;
}

template <
  typename T,
  typename U,
  FunctionConcept<bool, T, T> LessFunc,
  typename BeforeChangeFunc,
  typename AfterChangeFunc>
AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::Node*
AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::Balance(
  AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::Node* node
) {
	if (node == nullptr)
		return nullptr;
	if (Dist(length(node->child1), length(node->child2)) >= 2) {
		if (length(node->child1) > length(node->child2)) {
			// child 1 is longer => (child1 != nullptr)
			if (length(node->child1->child1) >= length(node->child1->child2)) {
				// ll case
				node = LL_Rotate(node);
			} else {
				// lr case
				node = LR_Rotate(node);
			}
		} else {
			// child 2 is longer
			if (length(node->child2->child2) >= length(node->child2->child1)) {
				// rr case
				node = RR_Rotate(node);
			} else {
				// rl case
				node = RL_Rotate(node);
			}
		}
	} else {
		Update(node);
	}
	return node;
}

template <
  typename T,
  typename U,
  FunctionConcept<bool, T, T> LessFunc,
  typename BeforeChangeFunc,
  typename AfterChangeFunc>
AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::Node*
AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::LL_Rotate(
  AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::Node* node
) {
	if (WithExtension) {
		before_change(node);
		before_change(node->child1);
	}

	Node* newtop = node->child1;
	node->child1 = newtop->child2;
	newtop->child2 = node;
	Update(node);
	Update(newtop);
	return newtop;
}

template <
  typename T,
  typename U,
  FunctionConcept<bool, T, T> LessFunc,
  typename BeforeChangeFunc,
  typename AfterChangeFunc>
AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::Node*
AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::LR_Rotate(
  AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::Node* node
) {
	if (WithExtension) {
		before_change(node);
		before_change(node->child1);
		before_change(node->child1->child2);
	}

	Node* newtop = node->child1->child2;
	node->child1->child2 = newtop->child1;
	newtop->child1 = node->child1;
	node->child1 = newtop->child2;
	newtop->child2 = node;
	Update(newtop->child1);
	Update(newtop->child2);
	Update(newtop);
	return newtop;
}

template <
  typename T,
  typename U,
  FunctionConcept<bool, T, T> LessFunc,
  typename BeforeChangeFunc,
  typename AfterChangeFunc>
AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::Node*
AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::RR_Rotate(
  AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::Node* node
) {
	if (WithExtension) {
		before_change(node);
		before_change(node->child2);
	}

	Node* newtop = node->child2;
	node->child2 = newtop->child1;
	newtop->child1 = node;
	Update(node);
	Update(newtop);
	return newtop;
}

template <
  typename T,
  typename U,
  FunctionConcept<bool, T, T> LessFunc,
  typename BeforeChangeFunc,
  typename AfterChangeFunc>
AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::Node*
AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::RL_Rotate(
  AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::Node* node
) {
	if (WithExtension) {
		before_change(node);
		before_change(node->child2);
		before_change(node->child2->child1);
	}
	Node* newtop = node->child2->child1;
	node->child2->child1 = newtop->child2;
	newtop->child2 = node->child2;
	node->child2 = newtop->child1;
	newtop->child1 = node;
	Update(newtop->child1);
	Update(newtop->child2);
	Update(newtop);
	return newtop;
}

template <
  typename T,
  typename U,
  FunctionConcept<bool, T, T> LessFunc,
  typename BeforeChangeFunc,
  typename AfterChangeFunc>
void AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::Update(
  AVLTree<T, U, LessFunc, BeforeChangeFunc, AfterChangeFunc>::Node* node
) {
	if (node == nullptr)
		return;
	node->weight = weight(node->child1) + weight(node->child2) + 1;
	node->length = Max(length(node->child1), length(node->child2)) + 1;

	if (WithExtension) {
		after_change(node);
	}
}
