#pragma once
#include "base.cpp"
#include "array.cpp"

template <typename T> inline T Min(T val1,T val2){
	if(val1<val2) return val1;
	return val2;
}

template <typename T> inline T Max(T val1,T val2){
	if(val1>val2) return val1;
	return val2;
}


template <typename T,typename... Args> inline T Min(T val1,Args... args){
	return Min(val1,Min(args...));
}



template <typename T,typename... Args> inline T Max(T val1,Args... args){
	return Max(val1,Max(args...));
}


template <typename T> inline void UpdateMin(T& val1,T val2){
	static_assert(std::is_copy_assignable<T>::value);
	if(val2<val1) val1=val2;
	return;
}
template <typename T> inline void UpdateMax(T& val1,T val2){
	static_assert(std::is_copy_assignable<T>::value);
	if(val2>val1) val1=val2;
	return;
}


template <typename T> T Max(Array<T> arr){
	if(arr.Length==0) return T();
	T max=arr[0];
	for(size_t i=1;i<arr.Length;++i){
		if(max<arr[i]) max=arr[i];
	}
	return max;
}

template <typename T> T Min(Array<T> arr){
	if(arr.Length==0) return T();
	T min=arr[0];
	for(size_t i=1;i<arr.Length;++i){
		if(min>arr[i]) min=arr[i];
	}
	return min;
}


