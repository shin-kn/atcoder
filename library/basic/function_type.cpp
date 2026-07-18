#pragma once

#include "base.cpp"

template <typename TResult,typename... TArgs> 
struct callable_base {
   virtual TResult operator()(TArgs...)=0;
   virtual ~callable_base(){}
};

template <typename... T> struct callable{

};

template <typename T,typename TResult,typename... TArgs>
struct callable<TResult(TArgs...),T>:callable_base<TResult,TArgs...> {
	T object;
	callable(T&& obj):object(std::move(obj)){}
	callable(T& obj):object(obj){}
    
	
	inline TResult operator()(TArgs... args){
		if constexpr (!std::is_void<TResult>::value) return object(std::forward<TArgs>(args)...);
		else object(std::forward<TArgs>(args)...);
	}
	
};

template<typename... T>
class FunctionType;


template <typename TResult,typename... TArgs> 
class FunctionType<TResult(TArgs...)> {
	public:
		callable_base<TResult,TArgs...>* Func=nullptr;
		FunctionType(){}
		FunctionType(TResult (*func) (TArgs...)){
			Func=new callable<TResult(TArgs...),TResult(*) (TArgs...)>(func);
		}

		template<typename T,typename std::enable_if<!std::is_same<FunctionType,T>::value,int>::type unused=0>
		FunctionType(T&& func){
			Func=new callable<TResult(TArgs...),typename std::remove_reference<T>::type>(std::forward<T>(func));
		}
		
		
		~FunctionType(){
            if(Func!=nullptr){
                delete Func;
            }
		}
	
		inline TResult operator () (TArgs... args){
		    if constexpr (!std::is_void<TResult>::value) return (*Func)(args...);
		    (*Func)(std::forward<TArgs>(args)...);
		}

	
		
		
		FunctionType(FunctionType& src)=delete;
		FunctionType(FunctionType&& src){
			Func=src.Func;
			src.Func=nullptr;
		}
		FunctionType& operator=(FunctionType& src)=delete;
		FunctionType& operator=(FunctionType&& src){
			if(this==&src){
				return *this;
			}
			if(Func!=nullptr) delete Func;
			Func=src.Func;
			src.Func=nullptr;
			return *this;
		}
	
		
		
	
};