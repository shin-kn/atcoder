#pragma once
#include "../basic.cpp"

template  <typename T,typename U> class AVLTree{
	static_assert(std::is_copy_assignable<T>::value);
	static_assert(std::is_copy_assignable<U>::value);
	public:
		AVLTree();
		ull Size=0;

		void Push(T,U);

		bool Find(T);
		bool Leq(T);
		bool Geq(T);
		ull Number(){return CurrentNumber;}

		void Delete();
		T Index(){return Current->index;}
		T Index(T ind){Find(ind);return Index();}
		U& Data(){return Current->val;}
		U& Data(T ind){Find(ind);return Data();}


		Pair<T,U&> operator [](ull ind);

		::Array<T> IndexArray();
		::Array<U> DataArray();
		::Array<Pair<T,U>> Array();





		template <typename V>
		AVLTree(V&& func){Init(std::forward<V>(func));}

		template <typename V>
		void Init(V&& func){is_small=FunctionType<bool(T,T)>(std::forward<V>(func));}


		//internal things


		FunctionType<bool(T,T)> is_small; 


		

		struct Node{
			Node* child1;
			Node* child2;
			ull length;//used for balancing, >=1
			ull weight;//used for an array like feature, >=1
			T index;
			U val;
			void Clear();
		};

		::Array<Node> Nodes;


		Node* Balance(Node* );
		Node* LL_Rotate(Node *);
		Node* LR_Rotate(Node *);
		Node* RL_Rotate(Node *);
		Node* RR_Rotate(Node *);


		void Update(Node* );

		Stack<Pair<Node*,bool>> FindRoutes; 

		//return top of the node
		//assume childrens of the node is balanced, and their weight and length are set correctly

		Stack<Node*> AvailNodes;
		Node* Root=nullptr;
		Node* Current;
		ull CurrentNumber; 

		void Search(FunctionType<void(Node*)>);

		template <typename F>
		void Search(F&& func){Search(FunctionType<void(Node*)>(std::forward<F>(func)));}


		//utils

		inline ull length(Node* node){return node==nullptr?0:node->length;}
		inline ull weight(Node* node){return node==nullptr?0:node->weight;}
		inline bool leq(T ind1,T ind2){return !is_small(ind2,ind1);}
		inline bool geq(T ind1,T ind2){return !is_small(ind1,ind2);}

};


template <typename T,typename U>
Pair<T,U&> AVLTree<T,U>::operator [](ull index){
	assert(index<Size);
	Node* loc=Root;
	ull counter=0;
	while(true){
		ull thisindex=weight(loc->child1)+counter;
		if(thisindex==index) break;
		if(thisindex<index){
			counter+=weight(loc->child1)+1;
			loc=loc->child2;
			continue;
		}
		loc=loc->child1;
	} 
	Current=loc;
	CurrentNumber=index;
	return Pair<T,U&>(loc->index,loc->val);
}


template <typename T,typename U>
::Array<Pair<T,U>> AVLTree<T,U>::Array(){
	::Array<Pair<T,U>> arr;
	arr.Allocate(Size);
	auto append=[&](Node* node){
		arr[arr.Length]=Pair<T,U>(node->index,node->val);	
	};
	Search(append);
	return arr;
}

template <typename T,typename U>
::Array<U> AVLTree<T,U>::DataArray(){
	::Array<U> arr;
	arr.Allocate(Size);
	auto append=[&](Node* node){
		arr[arr.Length]=node->val;
	};
	Search(append);
	return arr;
}

template <typename T,typename U>
::Array<T> AVLTree<T,U>::IndexArray(){
	::Array<T> arr;
	arr.Allocate(Size);
	auto append=[&](Node* node){
		arr[arr.Length]=node->index;
	};
	Search(append);
	return arr;
}



template <typename T,typename U>
void AVLTree<T,U>::Search(FunctionType<void(Node*)> func){
	if(Size==0) return;
	::Array<Node*,true> loc;
	::Array<ull,true> num;
	ull place=0;
	loc(0)=Root;
	num(0)=0;
	while(true){
		if(num[place]==2){
			if(place==0) break;
			--place;
			++num[place];
			continue;
		}
		if(num[place]==1){
			func(loc[place]);
		}
		Node* togo=num[place]==0?loc[place]->child1:loc[place]->child2;
		if(togo==nullptr){
			++num[place];
			continue;
		}
		++place;
		loc(place)=togo;
		num(place)=0;
	}
}

template <typename T,typename U>
AVLTree<T,U>::AVLTree(){
	if constexpr (SmallerDefined<T>){
		is_small=FunctionType<bool(T,T)>([](T val1,T val2)->bool{return val1<val2;});
	}
}



template <typename T,typename U>
void AVLTree<T,U>::Push(T index,U val){
	++Size;
	Node* newnode=nullptr;
	if(AvailNodes.Size>0){
		newnode=AvailNodes.Pop();
	}else{
		newnode=&Nodes[Nodes.Length];
	}
	newnode->Clear();
	newnode->index=index;
	newnode->val=val;
	
	CurrentNumber=0;
	Current=newnode;

	if(Root==nullptr){
		Root=newnode;
		return;
	}

	

	Node* loc=Root;
	Stack<Pair<Node*,bool>> routes;//child is onleft
	while(true){
		if(is_small(index,loc->index)){
			if(loc->child1==nullptr){
				loc->child1=newnode;
				break;
			}else{
				routes.Push(Pair<Node*,bool>(loc,true));
				loc=loc->child1;
			}
		}else{
			if(loc->child2==nullptr){
				CurrentNumber+=weight(loc->child1)+1;
				loc->child2=newnode;
				break;
			}else{
				routes.Push(Pair<Node*,bool>(loc,false));
				CurrentNumber+=weight(loc->child1)+1;
				loc=loc->child2;
			}
		}
	}
	loc=Balance(loc);
	while(routes.Size>0){
		Pair<Node*,bool> route=routes.Pop();
		Node* thisnode=route.val1;
		(route.val2?thisnode->child1:thisnode->child2)=loc;
		loc=Balance(thisnode);
	}
	Root=loc;

}

template <typename T,typename U>
bool AVLTree<T,U>::Leq(T index){
	Node* BestCurrent=nullptr;
	ull BestCurrentNumber=0;
	Current=Root;
	CurrentNumber=0;
	FindRoutes.Clear();
	while(true){
		if(leq(Current->index,index)){
			BestCurrent=Current;
			BestCurrentNumber=CurrentNumber+weight(Current->child1);
			FindRoutes.Push(Pair<Node*,bool>(Current,false));
			CurrentNumber+=weight(Current->child1)+1;
			if(Current->child2==nullptr) break;
			Current=Current->child2;
		}else{
			FindRoutes.Push(Pair<Node*,bool>(Current,true));
			if(Current->child1==nullptr) break;
			Current=Current->child1;
		}
	}
	if(BestCurrent==nullptr) return false;
	if(BestCurrent!=Current){
		while(true){
			if(FindRoutes.Pop().val1==BestCurrent) break;
		}
	}
	Current=BestCurrent;
	CurrentNumber=BestCurrentNumber;	
	return true;
}


template <typename T,typename U>
bool AVLTree<T,U>::Geq(T index){
	Node* BestCurrent=nullptr;
	ull BestCurrentNumber=0;
	Current=Root;
	CurrentNumber=0;
	FindRoutes.Clear();
	while(true){
		if(geq(Current->index,index)){
			BestCurrent=Current;
			BestCurrentNumber=CurrentNumber+weight(Current->child1);
			FindRoutes.Push(Pair<Node*,bool>(Current,true));
			if(Current->child1==nullptr) break;
			Current=Current->child1;
		}else{
			FindRoutes.Push(Pair<Node*,bool>(Current,false));
			if(Current->child2==nullptr) break;
			CurrentNumber+=weight(Current->child1)+1;
			Current=Current->child2;
		}
	}
	if(BestCurrent==nullptr) return false;
	if(BestCurrent!=Current){
		while(true){
			if(FindRoutes.Pop().val1==BestCurrent) break;
		}
	}
	Current=BestCurrent;
	CurrentNumber=BestCurrentNumber;	
	return true;
}

template <typename T,typename U>
bool AVLTree<T,U>::Find(T index){
	Current=Root;
	CurrentNumber=0;
	FindRoutes.Clear();
	while(Current!=nullptr){
		if(is_small(index,Current->index)){
			FindRoutes.Push(Pair<Node*,bool>(Current,true));
			Current=Current->child1;
			continue;
		}
		if(is_small(Current->index,index)){
			FindRoutes.Push(Pair<Node*,bool>(Current,false));
			CurrentNumber+=weight(Current->child1)+1;
			Current=Current->child2;
			continue;
		}
		CurrentNumber+=weight(Current->child1);
		break;
	}
	if(Current==nullptr){
		return false; 
	}
	return true;
}

template <typename T,typename U>
void AVLTree<T,U>::Delete(){
	if(Current==nullptr) return;
	--Size;
	AvailNodes.Push(Current);
	Node* loc=nullptr;
	
	if(Current->child1!=nullptr||Current->child2!=nullptr){
		if(length(Current->child1)>length(Current->child2)){
			//child1 is not nullptr
			Node* loc_parent;
			loc_parent=Current;
			loc=Current->child1;
			while(loc->child2!=nullptr){
				loc_parent=loc;
				loc=loc->child2;
			}
			if(loc_parent==Current){
				//loc=Current->child1
				loc->child2=Current->child2;
				Update(loc);
			}else{
				loc_parent->child2=loc->child1;
				loc->child1=Current->child1;
				loc->child2=Current->child2;
				FindRoutes.Push(Pair<Node*,bool>(loc,true));
				Current=Current->child1;
				while(Current!=loc_parent){
					FindRoutes.Push(Pair<Node*,bool>(Current,false));
					Current=Current->child2;
				}
				loc=Balance(loc_parent);
			}
		}else{
			//child2 is not nullptr
			Node* loc_parent;
			loc_parent=Current;
			loc=Current->child2;
			while(loc->child1!=nullptr){
				loc_parent=loc;
				loc=loc->child1;
			}
			if(loc_parent==Current){
				//loc=Current->child1
				loc->child1=Current->child1;
				Update(loc);
			}else{
				loc_parent->child1=loc->child2;
				loc->child1=Current->child1;
				loc->child2=Current->child2;
				FindRoutes.Push(Pair<Node*,bool>(loc,false));
				Current=Current->child2;
				while(Current!=loc_parent){
					FindRoutes.Push(Pair<Node*,bool>(Current,true));
					Current=Current->child1;
				}
				loc=Balance(loc_parent);
			}
		}
	}
	while(FindRoutes.Size>0){
		Pair<Node*,bool> route=FindRoutes.Pop();
		Node* thisnode=route.val1;
		(route.val2?thisnode->child1:thisnode->child2)=loc;
		loc=Balance(thisnode);
	}
	Root=loc;
	

}



template <typename T,typename U>
AVLTree<T,U>::Node* AVLTree<T,U>::Balance(AVLTree<T,U>::Node* node){
	if(node==nullptr) return nullptr;
	if(Dist(length(node->child1),length(node->child2))>=2){
		if(length(node->child1)>length(node->child2)){
			//child 1 is longer => (child1 != nullptr)
			if(length(node->child1->child1)>=length(node->child1->child2)){
				//ll case
				node=LL_Rotate(node);
			}else{
				//lr case
				node=LR_Rotate(node);
			}
		}else{
			//child 2 is longer
			if(length(node->child2->child2)>=length(node->child2->child1)){
				//rr case
				node=RR_Rotate(node);
			}else{
				//rl case
				node=RL_Rotate(node);
			}

		}
	}else{
		Update(node);
	}
	return node;
	



}

template <typename T,typename U> 
AVLTree<T,U>::Node* AVLTree<T,U>::LL_Rotate(AVLTree<T,U>::Node* node){
	Node* newtop=node->child1;
	node->child1=newtop->child2;
	newtop->child2=node;
	Update(node);
	Update(newtop);
	return newtop;
}

template <typename T,typename U> 
AVLTree<T,U>::Node* AVLTree<T,U>::LR_Rotate(AVLTree<T,U>::Node* node){
	Node* newtop=node->child1->child2;
	node->child1->child2=newtop->child1;
	newtop->child1=node->child1;
	node->child1=newtop->child2;
	newtop->child2=node;
	Update(newtop->child1);
	Update(newtop->child2);
	Update(newtop);
	return newtop;
}





template <typename T,typename U> 
AVLTree<T,U>::Node* AVLTree<T,U>::RR_Rotate(AVLTree<T,U>::Node* node){
	Node* newtop=node->child2;
	node->child2=newtop->child1;
	newtop->child1=node;
	Update(node);
	Update(newtop);
	return newtop;
}


template <typename T,typename U> 
AVLTree<T,U>::Node* AVLTree<T,U>::RL_Rotate(AVLTree<T,U>::Node* node){
	Node* newtop=node->child2->child1;
	node->child2->child1=newtop->child2;
	newtop->child2=node->child2;
	node->child2=newtop->child1;
	newtop->child1=node;
	Update(newtop->child1);
	Update(newtop->child2);
	Update(newtop);
	return newtop;
}

template <typename T, typename U>
void AVLTree<T,U>::Update(AVLTree<T,U>::Node* node){
	if(node==nullptr) return;
	node->weight=weight(node->child1)+weight(node->child2)+1;
	node->length=Max(length(node->child1),length(node->child2))+1;
}





template <typename T,typename U>
void AVLTree<T,U>::Node::Clear(){
	child1=nullptr;
	child2=nullptr;
	weight=1;
	length=1;
}
