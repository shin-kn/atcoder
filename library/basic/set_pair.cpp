#pragma once
#include "base.cpp"


template <typename T,size_t N> class Set {
	T Val[N];
	
	public:
		size_t Length=N;
		
		Set(){
		        
		}
		Set(Set& other){
			for(size_t i=0;i<N;++i) Val[i]=other.Val[i];
		}
		Set(Set&& other){
			for(size_t i=0;i<N;++i) Val[i]=other.Val[i];
		}
		
		template <typename... Args>
		void set(T head,Args... args){
		  Val[N-sizeof...(args)-1]=head;
			set(args...);
		}
		void set(){
			
		}
		
		template <typename... Args>
		Set(Args... args){
			set(args...);    
		}
		
		
		Set& operator=(const Set& src){
			for(size_t i=0;i<N;++i){
				Val[i]=src.Val[i];
			}
			return *this;
		}
		
		
		
		
		
		T& operator [](size_t idx){
			return Val[idx];
		}
		bool operator <(Set<T,N>& set){
			for(size_t i=0;i<N;++i){
				if(this->Val[i]<set.Val[i]){
					return true;
				}
				if(this->Val[i]>set.Val[i]){
					return false;
				}
			}
			return false;
		}
		bool operator >(Set<T,N>& set){
			for(size_t i=0;i<N;++i){
				if(this->Val[i]>set.Val[i]){
					return true;
				}
				if(this->Val[i]<set.Val[i]){
					return false;
				}
			}
			return false;
		}
		bool operator ==(Set<T,N>& set){
			for(size_t i=0;i<N;++i){
				if(this->Val[i]!=set.Val[i]){
					return false;
				}
			}
			return true;
		}
		
		
		
};


template <typename T1,typename T2> class Pair {
    static_assert(std::is_copy_assignable<T1>::value);
    static_assert(std::is_copy_assignable<T2>::value);
	
	public:
		Pair(T1 val1,T2 val2):val1(val1),val2(val2){
		}
		Pair(){
			
		}
		
		T1 val1;
		T2 val2;


		bool operator < (Pair other)requires SmallerDefined<T1> && SmallerDefined<T2>;
	
	
};

template <typename T1,typename T2> 
bool Pair<T1,T2>::operator < (Pair<T1,T2> other)
requires SmallerDefined<T1> && SmallerDefined<T2>{
	if(val1<other.val1|| other.val1< val1){
		return val1<other.val1;
	}
	return val2<other.val2;
}

template <typename T1,typename T2> std::ostream& operator << (std::ostream& os, Pair<T1,T2> val){
	os<<"("<<val.val1<<","<<val.val2<<")";
	return os;
}



