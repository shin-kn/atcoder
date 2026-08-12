#pragma once

#include "base.cpp"

template <typename... T> struct callable_base {};

template <typename TResult, typename... TArgs>
struct callable_base<TResult(TArgs...)> {
	virtual TResult operator()(TArgs...) = 0;
	virtual ~callable_base() {}

public:
	virtual callable_base* clone() = 0;
};

template <typename... T> struct callable {};

template <typename T, typename TResult, typename... TArgs>
struct callable<TResult(TArgs...), T> : callable_base<TResult(TArgs...)> {
	T object; // assume object to be copy initializable
	callable(T&& obj) : object(std::move(obj)) {}
	callable(T& obj) : object(obj) {}

	callable_base<TResult(TArgs...)>* clone() {
		callable* cloned = new callable(object);
		return cloned;
	}

	inline TResult operator()(TArgs... args) {
		if constexpr (!std::is_void<TResult>::value)
			return object(std::forward<TArgs>(args)...);
		else
			object(std::forward<TArgs>(args)...);
	}
};

template <typename... T> class FunctionType;

template <typename TResult, typename... TArgs>
class FunctionType<TResult(TArgs...)> {
public:
	callable_base<TResult(TArgs...)>* Func = nullptr;
	FunctionType() {}
	FunctionType(TResult (*func)(TArgs...)) {
		Func = new callable<TResult(TArgs...), TResult (*)(TArgs...)>(func);
	}

	template <typename T,
	          typename std::enable_if<!std::is_same<FunctionType, T>::value,
	                                  int>::type unused = 0>
	FunctionType(T&& func) {
		Func = new callable<TResult(TArgs...),
		                    typename std::remove_reference<T>::type>(
		    std::forward<T>(func));
	}

	~FunctionType() {
		if (Func != nullptr) {
			delete Func;
		}
	}

	inline TResult operator()(TArgs... args) {
		if constexpr (!std::is_void<TResult>::value)
			return (*Func)(args...);
		(*Func)(std::forward<TArgs>(args)...);
	}

	FunctionType(FunctionType& src) { Func = src.Func->clone(); }
	FunctionType(FunctionType&& src) {
		Func = src.Func;
		src.Func = nullptr;
	}
	FunctionType& operator=(FunctionType& src) = delete;
	FunctionType& operator=(FunctionType&& src) {
		if (this == &src) {
			return *this;
		}
		if (Func != nullptr)
			delete Func;
		Func = src.Func;
		src.Func = nullptr;
		return *this;
	}
};

template <typename T, typename TResult, typename... TArgs>
concept FunctionConcept = requires(T f, TArgs... args) {
	{ f(args...) } -> std::convertible_to<TResult>;
};
