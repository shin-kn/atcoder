#pragma once
#include "base.cpp"

template <typename T> class Stack {
	public:
		Array<T> Arr;
		size_t Size=0;
		void Push(T);
		T Pop();
		T Top();
		Stack(){}
		Stack(size_t n){
			Arr.Allocate(n);
		}
		void Clear();
		Stack(Stack&& other){
			Arr=std::move(other.Arr);
			Size=other.Size;
			other.Size=0;
		}
		Stack& operator = (Stack&& other){
			Arr=std::move(other.Arr);
			Size=other.Size;
			other.Size=0;
		}
		Stack(Stack& other)=delete;
		Stack& operator=(Stack& other)=delete;
		
};

template <typename T> inline void Stack<T>::Clear(){
	this->Size=0;
}

template <typename T> inline void Stack<T>::Push(T n){
	this->Arr[this->Size]=n;
	this->Size++;
}
template <typename T> inline T Stack<T>::Pop(){
	this->Size--;
	return this->Arr[this->Size];
}

template <typename T> inline T Stack<T>::Top(){
	return this->Arr[this->Size-1];
}

template <typename T> class Queue {
	Array<T> Arr;
	size_t length=0;
	size_t loc=0;
	void Refresh();
	public:
		size_t Size=0;
		void Push(T);
		T Pop();
		T Top();
		Queue(){}
		Queue(size_t n){
			Arr.Allocate(n);
		}
		void Clear();
};

template <typename T> inline void Queue<T>::Clear(){
	this->length=0;
	this->loc=0;
	this->Size=0;
}




template <typename T> inline void Queue<T>::Refresh(){
	for(size_t i=0;i+this->loc<this->length;i++){
		this->Arr[i]=this->Arr[this->loc+i];
	}
	this->loc=0;
	this->length=this->Size;
}

template <typename T> inline void Queue<T>::Push(T n){
	this->Arr[this->length]=n;
	this->length++;
	this->Size++;
	if(this->length-this->Size>=SomeBigSize){
		this->Refresh();
	}
}
template <typename T> inline T Queue<T>::Pop(){
	this->Size--;
	return this->Arr[this->loc++];
}

template <typename T> inline T Queue<T>::Top(){
	return this->Arr[this->loc];
}

