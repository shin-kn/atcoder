#pragma once
#include "../basic.cpp"

template <typename T> class Tree {
public:
	Tree() {}
	Tree(ull root) { Init(root); }

	void Init(ull root) { Root = root; };

	ull Root;
	void Connect(ull parent, ull child, T val);

	struct Node {
		Array<ull> childs;
		ull parent = 0;
		T toparent;
		LightArray<ull> parents; // by 2^n
		LightArray<T> toparents; // by 2^n
		ull depth = 0;           // root is 0
	};

	void DFS(
	  ull start,
	  FunctionType<void(ull)> backfunc,
	  FunctionType<bool(ull, ull)> cango,
	  FunctionType<void(ull)> firstreach
	);

	LightArray<ull> EulerTour(ull start);

	template <typename func1, typename func2, typename func3>
	void DFS(ull start, func1&& backfunc, func2&& cango, func3&& firstreach) {
		DFS(
		  start, FunctionType<void(ull)>(std::forward<func1>(backfunc)),
		  FunctionType<bool(ull, ull)>(std::forward<func2>(cango)),
		  FunctionType<void(ull)>(std::forward<func3>(firstreach))
		);
	}

	Array<Node, true> Nodes;

	ull LCA(ull, ull);
	T Distance(ull, ull);

	// internal
	bool canLCA = false;
	void Memo();
	ull JumpParent(ull, ull); // calld on assumption that canLCA=true
	ull JumpParentWithDeg(
	  ull, ull
	);                        // jump 2^deg; calld on assumption that canLCA=true
	T JumpToParent(ull, ull); // calld on assumption that canLCA=true
	                          // assume that T() works like 0
};

template <typename T> void Tree<T>::Connect(ull parent, ull child, T val) {
	Nodes(parent).childs.Push(child);
	Nodes(child).parent = parent;
	Nodes(child).toparent = val;
}

template <typename T> LightArray<ull> Tree<T>::EulerTour(ull start) {
	// each node is pushed twice (enter and leave), so 2*Nodes.Length is enough
	LightArray<ull> tour(2 * Nodes.Length);
	ull length = 0;
	auto push_tour = [&](ull n) {
		tour[length] = n;
		++length;
	};
	auto cango = [](ull, ull) -> bool { return true; };
	DFS(start, push_tour, cango, push_tour);
	tour.Length = length;
	return tour;
}

template <typename T>
void Tree<T>::DFS(
  ull start,
  FunctionType<void(ull)> backfunc,
  FunctionType<bool(ull, ull)> cango,
  FunctionType<void(ull)> firstreach
) {
	// stack depth <= number of nodes, since a tree path never repeats a node
	LightArray<ull> loc(Nodes.Length + 1);
	LightArray<ull> num(Nodes.Length + 1);
	ull place = 0;
	loc(0) = start;
	num(0) = 0;
	firstreach(start);
	while (true) {
		ull locnode = loc[place];
		if (Nodes[locnode].childs.Length == num[place]) {
			backfunc(locnode);
			if (place == 0)
				break;
			--place;
			++num[place];
			continue;
		}
		ull dest = Nodes[locnode].childs[num[place]];
		if (!cango(locnode, dest)) {
			++num[place];
			continue;
		}
		firstreach(dest);
		++place;
		loc(place) = dest;
		num(place) = 0;
	}
}

template <typename T> ull Tree<T>::LCA(ull node1, ull node2) {
	if (!canLCA)
		Memo();
	if (Nodes[node1].depth < Nodes[node2].depth)
		Swap(node1, node2);
	node1 = JumpParent(node1, Nodes[node1].depth - Nodes[node2].depth);
	if (node1 == Root)
		return node1;
	ull depth = Nodes[node1].depth;
	ull degmax = Log2(SmallerPower2(depth));
	if (node1 == node2)
		return node1;
	ull succeed = Root;
	for (ull deg = degmax;; --deg) {
		if (Nodes[node1].depth < (1ull << deg))
			continue;
		ull newnode1 = JumpParentWithDeg(node1, deg);
		ull newnode2 = JumpParentWithDeg(node2, deg);
		if (newnode1 == newnode2) {
			succeed = newnode1;
		} else {
			node1 = newnode1;
			node2 = newnode2;
		}

		if (deg == 0)
			break;
	}
	return succeed;
}

template <typename T> T Tree<T>::Distance(ull node1, ull node2) {
	if (!canLCA)
		Memo();
	ull ancestor = LCA(node1, node2);
	return JumpToParent(node1, Nodes[node1].depth - Nodes[ancestor].depth) +
	       JumpToParent(node2, Nodes[node2].depth - Nodes[ancestor].depth);
}

template <typename T> void Tree<T>::Memo() {
	if (canLCA)
		return;
	canLCA = true;
	// non-root nodes; filtered in place as degree grows
	LightArray<ull> stack(Nodes.Length);
	ull stack_len = 0;
	{
		auto firstreach = [&](ull node) {};
		auto cango = [&](ull from, ull to) -> bool {
			stack[stack_len] = to;
			++stack_len;
			Node& node = Nodes[to];
			node.depth = Nodes[from].depth + 1;
			// parents[deg] exists iff 2^deg <= depth
			ull levels = Log2(SmallerPower2(node.depth)) + 1;
			node.parents.Allocate(levels);
			node.toparents.Allocate(levels);
			node.parents[0] = node.parent;
			node.toparents[0] = node.toparent;
			return true;
		};
		auto backfunc = [&](ull) {};
		DFS(Root, backfunc, cango, firstreach);
	}

	for (ull degree = 1;; ++degree) {
		ull deglen = (1ull << degree);
		ull new_len = 0;
		for (ull i = 0; i < stack_len; ++i) {
			Node& locnode = Nodes[stack[i]];
			if (deglen > locnode.depth) {
				continue;
			}
			stack[new_len] = stack[i];
			++new_len;
			locnode.parents[degree] =
			  Nodes[locnode.parents[degree - 1]].parents[degree - 1];
			locnode.toparents[degree] =
			  locnode.toparents[degree - 1] +
			  Nodes[locnode.parents[degree - 1]].toparents[degree - 1];
		}
		if (new_len == 0)
			break;
		stack_len = new_len;
	}
}

template <typename T> ull Tree<T>::JumpParent(ull node, ull jump) {
	if (jump == 0)
		return node;
	ull degmax = Log2(SmallerPower2(jump));
	for (ull deg = 0; deg <= degmax; ++deg) {
		if (jump & (1ull << deg)) {
			node = Nodes[node].parents[deg];
		}
	}
	return node;
}

template <typename T> ull Tree<T>::JumpParentWithDeg(ull node, ull deg) {
	// assume depth of node >= 2^deg
	return Nodes[node].parents[deg];
}

template <typename T> T Tree<T>::JumpToParent(ull node, ull jump) {
	if (jump == 0)
		return T();
	ull degmax = Log2(SmallerPower2(jump));
	T counter = T();
	for (ull deg = 0; deg <= degmax; ++deg) {
		if (jump & (1ull << deg)) {
			counter = counter + Nodes[node].toparents[deg];
			node = Nodes[node].parents[deg];
		}
	}
	return counter;
}
