#pragma once
#include "base.cpp"

constexpr inline size_t BiggerPower2(size_t num){
	if(num==0) return 1;
	int dig=64-std::countl_zero(num)-1;
	if(((size_t)1<<dig)==num) return num;
	return ((size_t)1<<(dig+1));
}






constexpr inline size_t SmallerPower2(size_t num){//num must not be 0
	if(num==0) return 0;
	int dig=64-std::countl_zero(num)-1;
	return ((size_t)1<<dig);
}

template <typename T>
constexpr inline T Log2(T num){//
	size_t counter=1;
	size_t i=0;
	for(;true;i++){
		if(counter>=num) break;
		counter=counter<<1;
	}
	return i;
}

template<>
constexpr inline size_t Log2<size_t>(size_t num){
	return static_cast<size_t>(64-std::countl_zero(num)-1);
}



template <typename T> constexpr T Power(T val,size_t times){
	if(times==0){
		return One<T>; 
	}
	
	T powers[64];
	size_t maxdeg=Log2(SmallerPower2(times));
	powers[0]=val;
	for(size_t i=1;i<=maxdeg;++i){
		powers[i]=powers[i-1]*powers[i-1];
	}
	T res=One<T>;
	for(size_t i=0;i<=maxdeg;++i){
		if(times&((size_t)1<<i)){
			res=res*powers[i];
		}
	}
	return res;
	
	
}





