
#pragma once
#include "../basic.cpp"

template <typename T,typename U> 
class AVLArray{
    static_assert(std::is_copy_assignable<T>::value);
    static_assert(std::is_copy_assignable<U>::value);
    public:
        AVLArray();

        template <typename... Args>
        AVLArray(Args&&... args){Init(std::forward<Args>(args)...);}

		template <typename V,typename W>
		void Init(V&& index_small,W&& data_small);

        //index updating
        U& operator [](T ind);
        U& Nth(ull ind);
        void Delete();
        void Delete(T ind);
        void DeleteIfExists(T ind);
        //not index updating
        T Index(){return index;}
        bool Exists(T ind);

        //index updating
        U& Min(){return Nth(0);}
        U& Max(){return Nth(Length-1);}

        ull Length=0;



        ::Array<T> IndexArray(){return index_tree.IndexArray();}
        ::Array<Pair<T,U>> Array(){return index_tree.Array();}
        ::Array<U> DataArray();

        //internal things

        bool queued=false;
        T index;
        U* queuePointer=nullptr;
        U originalValue;


        AVLTree<T,U> index_tree;
        AVLTree<Pair<U,T>,T> data_tree; 

        FunctionType<bool(T,T)> index_small;
        FunctionType<bool(U,U)> data_small;

        void Resolve();

        bool neq(U n1,U n2){return data_small(n1,n2) || data_small(n2,n1);}


};


template <typename T, typename U>
::Array<U> AVLArray<T,U>::DataArray(){
    ::Array<U> arr(Length);
    ::Array<Pair<T,U>> pair_arr=data_tree.IndexArray();
    for(ull i=0;i<pair_arr.Length;++i){
        arr[i]=pair_arr[i].val1;
    }
    return arr;
}


template <typename T, typename U>
AVLArray<T,U>::AVLArray(){
    if constexpr (SmallerDefined<T>){
        index_small=SmallerFunc<T>();
    }
    if constexpr (SmallerDefined<U>){
        data_small=SmallerFunc<U>();
    }
}


template <typename T,typename U>
template <typename V,typename W>
void AVLArray<T,U>::Init(V&& index_small_func,W&& data_small_func){
    index_small=FunctionType<bool(T,T)>(std::forward<V>(index_small_func));
    data_small=FunctionType<bool(U,U)>(std::forward<W>(data_small_func));
    index_tree.Init(index_small);
    data_tree.Init([&](Pair<U,T> n1,Pair<U,T> n2)->bool{
        if(data_small(n1.val1,n2.val1) || data_small(n2.val1,n1.val1)){
            return data_small(n1.val,n2.val);
        }
        return index_small(n1.val2,n2.val2);
    });
}

template <typename T,typename U>
void AVLArray<T,U>::Resolve(){
    if(queued){
        if(neq(originalValue,*queuePointer)){
            data_tree.Find(Pair<U,T>(originalValue,index));
            data_tree.Delete();
            data_tree.Push(Pair<U,T>(*queuePointer,index),index);
        }
        queued=false;
    }
}



template<typename T,typename U>
U& AVLArray<T,U>::operator [](T ind){
    Resolve();
    index=ind;
    if(!index_tree.Find(ind)){
        ++Length;
        U initval=U();
        index_tree.Push(ind,initval);
        data_tree.Push(Pair<U,T>(initval,index),index);
    }
    originalValue=index_tree.Data();
    queuePointer=&(index_tree.Data());
    queued=true;
    return index_tree.Data();
}



template<typename T,typename U>
void AVLArray<T,U>::Delete(){
    assert(queued);
    queued=false;
    --Length;
    index_tree.Find(index);
    index_tree.Delete();
    data_tree.Find(Pair<U,T>(originalValue,index));
    data_tree.Delete();
}


template<typename T,typename U>
void AVLArray<T,U>::DeleteIfExists(T ind){
    if(Exists(ind)){
        Delete(ind);
    }
}

template<typename T,typename U>
void AVLArray<T,U>::Delete(T ind){
    (*this)[ind];
    Delete();
}


template<typename T,typename U>
bool AVLArray<T,U>::Exists(T ind){
    return index_tree.Find(ind);
}

template<typename T,typename U>
U& AVLArray<T,U>::Nth(ull ind){
    assert(ind<Length);
    Resolve();
    data_tree[ind];
    index=data_tree.Data();
    queued=true;
    index_tree.Find(index);
    originalValue=index_tree.Data();
    queuePointer=&(index_tree.Data());
    return index_tree.Data();
}


