#pragma once
#include "base.cpp"
#include "array.cpp"
#include "math.cpp"


template <std::unsigned_integral T> inline T Abs(T num){//it is unsafe a little
	return Min(num,num*T(-1));	
}

template <std::signed_integral T> inline T Abs(T num){
	if(num<T(0)) return num*T(-1);
	return num;
}

template <std::integral T> inline T Dist(T n1,T n2){
	if(n1>n2){
		return n1-n2;
	}
	return n2-n1;
}







Array<size_t> ToBinary(size_t num){
	auto log=Log2(BiggerPower2(num+1));
	Array<size_t> arr(log);
	for(size_t i=0;i<log;++i){
		arr[i]=(1<<i)&num?1:0;
	}
	return arr;
}


//About 2^N tree system
/*
					0
				  |
				1 - 2
				|   |
			 3-4 5-6


*/


constexpr inline size_t Parent(size_t n) {return ((n+1)>>1)-1;}//don't call to 0
constexpr inline size_t Child(size_t n) {return ((n+1)<<1)-1;}









template <typename T> inline void Swap(T& val1,T& val2){
	T temp=val1;
	val1=val2;
	val2=temp;
}


