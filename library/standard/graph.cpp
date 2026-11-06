#pragma once
#include "../basic/array.cpp"
#include "../basic/base.cpp"
#include "../basic/function_type.cpp"
#include "../basic/light_array.cpp"
#include "../basic/min_max.cpp"
#include "../basic/stack_queue.cpp"
#include "tree.cpp"

template <typename T = ull, typename U = ull> class Graph {
public:
	Graph() {}

	void Push(ull n) { Nodes[n].IsNull = false; }
	void Connect(ull, ull);
	void Connect(ull, ull, T);
	void Connect(ull, ull, T, U);

	Array<T> PathCapacity;
	Array<U> PathCost;

	struct Node {
		bool IsNull = true;
		Array<ull> FromPath;
		Array<ull> ToPath;
	};
	Array<Node> Nodes;

	struct Edge {
		ull From;
		ull To;
	};
	Array<Edge> Edges;

	void Clear();

	template <bool Forward = true>
	void DFS(
	  ull start,
	  FunctionType<void(ull)> backfunc,
	  FunctionType<bool(ull)> cango,
	  FunctionType<void(ull)> firstreach
	);

	template <typename T2, typename U2> Array<ull> SCC(Graph<T2, U2>&);

	Tree<T> GenerateTree(ull root);
	// assume that graph is undirected tree(or directed from root)

	T Dinitz(ull start, ull end); // max flow with PathCapacity, O(V^2 E)
	LightArray<T> Flow;           // flow on each edge, set by Dinitz

	// internal
};

template <typename T, typename U> void Graph<T, U>::Clear() {
	Nodes = Array<Node>();
	Edges = Array<Edge>();
}

template <typename T, typename U> void Graph<T, U>::Connect(ull from, ull to) {
	ull index = Edges.Length;
	Edges.Allocate(index + 1);
	Edges.Length = index + 1;
	Edges(index).From = from;
	Edges(index).To = to;
	Nodes(from).FromPath.Push(index);
	Nodes(to).ToPath.Push(index);
}

template <typename T, typename U>
void Graph<T, U>::Connect(ull from, ull to, T cap) {
	Connect(from, to);
	PathCapacity[Edges.Length - 1] = cap;
}

template <typename T, typename U>
void Graph<T, U>::Connect(ull from, ull to, T cap, U cost) {
	Connect(from, to, cap);
	PathCost[Edges.Length - 1] = cost;
}

template <typename T, typename U>
template <bool Forward>
void Graph<T, U>::DFS(
  ull start,
  FunctionType<void(ull)> backfunc,
  FunctionType<bool(ull)> cango,
  FunctionType<void(ull)> firstreach
) {
	Array<ull> loc;
	Array<ull> num;
	ull place = 0;
	loc[0] = start;
	num[0] = 0;
	firstreach(start);
	while (true) {
		if (
		  (Forward && Nodes(loc(place)).FromPath.Length == num(place)) ||
		  (!Forward && Nodes(loc(place)).ToPath.Length == num(place))
		) {
			backfunc(loc(place));
			if (place == 0)
				break;
			--place;
			++num(place);
			continue;
		}
		// check path
		ull path;
		if constexpr (Forward) {
			path = (Nodes(loc(place)).FromPath(num(place)));
		} else {
			path = (Nodes(loc(place)).ToPath(num(place)));
		}
		if (!cango(path)) {
			++num(place);
			continue;
		}
		ull dest;
		if constexpr (Forward) {
			dest = Edges(path).To;
		} else {
			dest = Edges(path).From;
		}
		firstreach(dest);
		++place;
		loc[place] = dest;
		num[place] = 0;
	}
}

template <typename T, typename U>
template <typename T2, typename U2>
Array<ull> Graph<T, U>::SCC(Graph<T2, U2>& newgraph) {
	newgraph.Clear();

	LightArray<bool> reached(Nodes.Length, false);
	LightArray<bool> grouped(Nodes.Length, false);

	LightArray<ull> order(Nodes.Length);
	LightArray<ull> lowlink(Nodes.Length);
	LightArray<ull> parent(Nodes.Length);
	Array<ull> res;
	ull res_index = 0;
	Stack<ull> stack;
	ull order_index = 0;
	auto backfunc = [&](ull n) {
		if (lowlink[n] == order[n]) {
			while (true) {
				ull loc = stack.Pop();
				res[loc] = res_index;
				grouped[loc] = true;
				if (loc == n)
					break;
			}
			++res_index;
		}
		if (stack.Size != 0) {
			UpdateMin(lowlink[parent[n]], lowlink[n]);
		}
	};
	auto cango = [&](ull n) -> bool {
		if (!reached[Edges(n).To]) {
			parent[Edges(n).To] = Edges(n).From;
			return true;
		} else {
			if (!grouped[Edges(n).To]) {
				UpdateMin(lowlink[Edges(n).From], lowlink[Edges(n).To]);
			}
		}
		return false;
	};
	auto firstreach = [&](ull n) {
		order[n] = order_index;
		lowlink[n] = order_index;
		++order_index;
		reached[n] = true;
		stack.Push(n);
	};
	for (ull i = 0; i < Nodes.Length; ++i) {
		if (!Nodes(i).IsNull && !reached[i]) {
			DFS(i, backfunc, cango, firstreach);
		}
	}
	for (ull i = 0; i < res_index; ++i)
		newgraph.Push(i);
	for (ull i = 0; i < Edges.Length; ++i) {
		newgraph.Connect(res[Edges(i).From], res[Edges(i).To]);
	}
	return res;
}

template <typename T, typename U> Tree<T> Graph<T, U>::GenerateTree(ull root) {
	Tree<T> tree(root);
	LightArray<bool> reached(Nodes.Length, false);
	auto backfunc = [&](ull) { return; };
	auto cango = [&](ull path) -> bool {
		if (reached[Edges(path).To])
			return false;
		// unweighted edges (Connect without cap) have no PathCapacity entry
		T val = path < PathCapacity.Length ? PathCapacity(path) : T();
		tree.Connect(Edges(path).From, Edges(path).To, val);
		return true;
	};
	auto firstreach = [&](ull node) { reached[node] = true; };
	DFS(root, backfunc, cango, firstreach);
	return tree;
};

// residual graph: each edge goes forward with PathCapacity - Flow, and
// backward with Flow
template <typename T, typename U> T Graph<T, U>::Dinitz(ull start, ull end) {
	Flow.Allocate(Edges.Length);
	Flow.Set(T(0));
	if (start == end)
		return T(0);

	// upper bound of the flow out of start
	T maxcap = T(0);
	for (ull path : Nodes(start).FromPath)
		maxcap = maxcap + PathCapacity(path);
	for (ull path : Nodes(start).ToPath)
		maxcap = maxcap + PathCapacity(path);

	// distance to end in the residual graph, -1 if unreachable or a dead end
	LightArray<ll> dist(Nodes.Length);

	// pushes up to cap along edges that get one step closer to end
	auto dfs = [&](auto& self, ull node, T cap) -> T {
		if (node == end || cap == T(0))
			return cap;
		T initcap = cap;
		ll level = dist[node];
		for (ull path : Nodes(node).FromPath) {
			if (cap == T(0))
				break;
			ull next = Edges(path).To;
			if (dist[next] != level - 1 || !(Flow[path] < PathCapacity(path)))
				continue;
			T d = self(self, next, Min(cap, PathCapacity(path) - Flow[path]));
			if (d == T(0))
				dist[next] = -1;
			cap = cap - d;
			Flow[path] = Flow[path] + d;
		}
		for (ull path : Nodes(node).ToPath) {
			if (cap == T(0))
				break;
			ull next = Edges(path).From;
			if (dist[next] != level - 1 || !(T(0) < Flow[path]))
				continue;
			T d = self(self, next, Min(cap, Flow[path]));
			if (d == T(0))
				dist[next] = -1;
			cap = cap - d;
			Flow[path] = Flow[path] - d;
		}
		return initcap - cap;
	};

	while (true) {
		// BFS from end, following residual edges backward
		dist.Set(-1);
		dist[end] = 0;
		Queue<ull> queue;
		queue.Push(end);
		while (queue.Size > 0 && dist[start] == -1) {
			ull node = queue.Pop();
			for (ull path : Nodes(node).ToPath) {
				ull prev = Edges(path).From;
				if (dist[prev] == -1 && Flow[path] < PathCapacity(path)) {
					dist[prev] = dist[node] + 1;
					queue.Push(prev);
				}
			}
			for (ull path : Nodes(node).FromPath) {
				ull prev = Edges(path).To;
				if (dist[prev] == -1 && T(0) < Flow[path]) {
					dist[prev] = dist[node] + 1;
					queue.Push(prev);
				}
			}
		}
		if (dist[start] == -1)
			break;
		dfs(dfs, start, maxcap);
	}

	T res = T(0);
	for (ull path : Nodes(start).FromPath)
		res = res + Flow[path];
	for (ull path : Nodes(start).ToPath)
		res = res - Flow[path];
	return res;
}
