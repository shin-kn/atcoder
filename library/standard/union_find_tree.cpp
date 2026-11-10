#pragma once
#include "../basic/array.cpp"
#include "../basic/base.cpp"
#include "../basic/stack_queue.cpp"
#include "../basic/utils.cpp"

class UnionFindTree { // E,V

	Array<size_t> Parent;
	Array<bool> IsRoot;
	Stack<size_t> stack;
	Array<bool> pushed;
	Array<ull> Size;

public:
	size_t RootNum = 0;

	void Push(ull ind) {
		for (ull i = pushed.Length; i <= ind; ++i)
			pushed[i] = false;
		if (pushed[ind])
			return;
		Parent[ind] = ind;
		IsRoot[ind] = true;
		pushed[ind] = true;
		Size[ind] = 1;
		++RootNum;
	}

	void Connect(size_t, size_t);
	bool IsConnected(size_t, size_t);
	size_t RootOf(size_t);
};

inline size_t UnionFindTree::RootOf(size_t n) {
	Push(n);
	while (true) {
		if (IsRoot[n])
			break;
		stack.Push(n);
		n = Parent[n];
	}
	while (stack.Size > 0) {
		Parent[stack.Pop()] = n;
	}
	return n;
}

inline void UnionFindTree::Connect(size_t n1, size_t n2) {
	ull n1_root = RootOf(n1);
	ull n2_root = RootOf(n2);
	if (n1_root == n2_root)
		return;

	if (Size[n1_root] < Size[n2_root])
		Swap(n1_root, n2_root);
	IsRoot[n2_root] = false;
	Parent[n2_root] = n1_root;
	Size[n1_root] += Size[n2_root];
	--RootNum;
}

inline bool UnionFindTree::IsConnected(size_t n1, size_t n2) {
	ull n1_root = RootOf(n1);
	ull n2_root = RootOf(n2);
	return n1_root == n2_root;
}
