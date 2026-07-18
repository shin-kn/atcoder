#pragma once
#include "../basic.cpp"
#include "tree.cpp"


template <typename T=ull, typename U=ull>
class Graph{
	public:
		Graph(){}
		
		void Push(ull n){Nodes(n).IsNull=false;}
		void Connect(ull,ull);
		void Connect(ull,ull,T);
		void Connect(ull,ull,T,U);
		
		Array<T,true> PathCapacity;
		Array<U,true> PathCost;

		struct Node{
			bool IsNull=true;
			Array<ull> FromPath;
			Array<ull> ToPath;
		};
		Array<Node,true> Nodes;

		struct Edge{
			ull From;
			ull To;
		};
		Array<Edge,true> Edges;

		void Clear();

		template <bool Forward=true>
		void DFS(ull start,FunctionType<void(ull)> backfunc,FunctionType<bool(ull)> cango,FunctionType<void(ull)> firstreach);


		template <bool Forward=true,typename func1,typename func2,typename func3>
		void DFS(ull start,func1&& backfunc, func2&& cango,func3&& firstreach){DFS<Forward>(start,std::forward<func1>(backfunc),std::forward<func2>(cango),std::forward<func3>(firstreach));}


		
		template<typename T2,typename U2>
		Array<ull> SCC(Graph<T2,U2>&);
		
		Tree<T> GenerateTree(ull root);
		//assume that graph is undirected tree(or directed from root)


		//internal
		

};

template <typename T, typename U> void Graph<T,U>::Clear(){
	Nodes=Array<Node>();
	Edges=Array<Edge>();
}

template <typename T, typename U> void Graph<T,U>::Connect(ull from, ull to){
	ull index=Edges.Length;
	Edges.Allocate(index+1);
	Edges.Length=index+1;
	Edges[index].From=from;
	Edges[index].To=to;
	Nodes[from].FromPath.Push(index);
	Nodes[to].ToPath.Push(index);
}

template <typename T, typename U> void Graph<T,U>::Connect(ull from, ull to, T cap){
	Connect(from,to);
	PathCapacity(Edges.Length-1)=cap;
}

template <typename T, typename U> void Graph<T,U>::Connect(ull from, ull to, T cap, U cost){
	Connect(from,to,cap);
	PathCost(Edges.Length-1)=cost;
}


template <typename T, typename U> 
template <bool Forward>
void Graph<T,U>::DFS(ull start,FunctionType<void(ull)> backfunc,FunctionType<bool(ull)> cango,FunctionType<void(ull)> firstreach){
	Array<ull,true> loc;
	Array<ull,true> num;
	ull place=0;
	loc(0)=start;
	num(0)=0;
	firstreach(start);
	while(true){
		if((Forward&&Nodes[loc[place]].FromPath.Length==num[place])||(!Forward&&Nodes[loc[place]].ToPath.Length==num[place])){
			backfunc(loc[place]);
			if(place==0) break;
			--place;
			++num[place];
			continue;
		}
		//check path
		ull path;
		if constexpr (Forward) {
			path=(Nodes[loc[place]].FromPath(num[place]));
		}else{
			path=(Nodes[loc[place]].ToPath(num[place]));
		}
		if(!cango(path)){
			++num[place];
			continue;
		}
		ull dest;
		if constexpr (Forward){
			dest=Edges[path].To;
		}else{
			dest=Edges[path].From;
		}
		firstreach(dest);
		++place;
		loc(place)=dest;
		num(place)=0;
	}


}


template <typename T, typename U>
template <typename T2,typename U2> Array<ull> Graph<T,U>::SCC(Graph<T2,U2>& newgraph){
	newgraph.Clear();

	LightArray<bool> reached(Nodes.Length,false);
	LightArray<bool> grouped(Nodes.Length,false);

	LightArray<ull> order(Nodes.Length);
	LightArray<ull> lowlink(Nodes.Length);
	LightArray<ull> parent(Nodes.Length);
	Array<ull> res;
	ull res_index=0;
	Stack<ull> stack;
	ull order_index=0;
	auto backfunc=[&](ull n){
		if(lowlink[n]==order[n]){
			while(true){
				ull loc=stack.Pop();
				res[loc]=res_index;
				grouped[loc]=true;
				if(loc==n) break;
			}
			++res_index;
		}
		if(stack.Size!=0){
			UpdateMin(lowlink[parent[n]],lowlink[n]);
		}	
	};
	auto cango=[&](ull n)->bool{
		if(!reached[Edges[n].To]){
			parent[Edges[n].To]=Edges[n].From;
			return true;
		}
		else{
			if(!grouped[Edges[n].To]){
				UpdateMin(lowlink[Edges[n].From],lowlink[Edges[n].To]);
			}
		}
		return false;
	};
	auto firstreach=[&](ull n){
		order[n]=order_index;
		lowlink[n]=order_index;
		++order_index;
		reached[n]=true;
		stack.Push(n);
	};
	for(ull i=0;i<Nodes.Length;++i){
		if(!Nodes[i].IsNull&&!reached[i]){
			DFS(i,backfunc,cango,firstreach);
		}
	}
	for(ull i=0;i<res_index;++i) newgraph.Push(i);
	return res;
	for(ull i=0;i<Edges.Length;++i){
		newgraph.Connect(res[Edges[i].From],res[Edges[i].To]);
	}
	return res;
}



template <typename T,typename U>
Tree<T> Graph<T,U>::GenerateTree(ull root){
	Tree<T> tree(root);
	Array<bool> reached(Nodes.Length,false);
	auto backfunc=[&](ull){return;};
	auto cango=[&](ull path)->bool{
		if(reached[Edges[path].To]) return false;
		tree.Connect(Edges[path].From,Edges[path].To,PathCapacity[path]);
		return true;
	};
	auto firstreach=[&](ull node){
		reached[node]=true;
	};
	DFS(root,backfunc,cango,firstreach);
	return tree;
};