#pragma once
#include "../basic.cpp"

class UnionFindTree { // E,V

	Array<size_t> Parent;
	Array<bool> IsRoot;
	Stack<size_t> stack;

public:
	size_t RootNum = 0;
	UnionFindTree(size_t N) {
		Parent.Allocate(N + 1);
		IsRoot.Set(0, N + 1, true);
	}
	void Connect(size_t, size_t);
	bool IsConnected(size_t, size_t);
	size_t RootOf(size_t);
};

inline size_t UnionFindTree::RootOf(size_t n) {
	Stack<size_t> stack;
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
	size_t loc = n1;
	while (true) {
		if (IsRoot[loc])
			break;
		stack.Push(loc);
		loc = Parent[loc];
	}
	size_t root = loc;
	loc = n2;
	while (true) {
		if (IsRoot[loc]) {
			if (root != loc) {
				IsRoot[loc] = false;
				--RootNum;
				stack.Push(loc);
			}
			break;
		}
		stack.Push(loc);
		loc = Parent[loc];
	}
	while (stack.Size > 0) {
		Parent[stack.Pop()] = root;
	}
}

inline bool UnionFindTree::IsConnected(size_t n1, size_t n2) {
	size_t loc = n1;
	while (true) {
		if (IsRoot[loc])
			break;
		stack.Push(loc);
		loc = Parent[loc];
	}
	size_t n1root = loc;
	while (stack.Size > 0) {
		Parent[stack.Pop()] = n1root;
	}

	loc = n2;
	while (true) {
		if (IsRoot[loc])
			break;
		stack.Push(loc);
		loc = Parent[loc];
	}
	size_t n2root = loc;
	while (stack.Size > 0) {
		Parent[stack.Pop()] = n2root;
	}
	return n1root == n2root;
}
