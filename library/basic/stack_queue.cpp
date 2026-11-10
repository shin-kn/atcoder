#pragma once
#include "array.cpp"
#include "base.cpp"
#include "light_array.cpp"
#include "min_max.cpp"

template <typename T> class FastStack {
public:
	::LightArray<T> arr;
	ull Size = 0;
	ull Capacity = 0;
	FastStack() {}
	FastStack(size_t n) : Capacity(n) { arr.Allocate(n); }
	void Push(T n) {
		if (Capacity == Size) {
			Capacity = Max(ull(2), Capacity * 2);
			LightArray<T> newarr(Capacity);
			for (ull i = 0; i < Size; ++i)
				newarr[i] = std::move(arr[i]);
			arr = std::move(newarr);
		}
		arr[Size] = n;
		++Size;
	}
	T Pop() {
		--Size;
		return arr[Size];
	}
	T Top() { return arr[Size - 1]; }
	void Clear() { Size = 0; }
};

template <typename T> class Stack {
public:
	::Array<T> arr;
	size_t Size = 0;
	Stack() {}
	Stack(size_t n) { arr.Allocate(n); }
	void Push(T n) {
		arr[Size] = n;
		++Size;
	}
	T Pop() {
		--Size;
		return arr(Size);
	}
	T Top() { return arr(Size - 1); }
	void Clear() { Size = 0; }
};

template <typename T> class Queue {
	Array<T> arr;
	size_t length = 0;
	size_t loc = 0;
	void Refresh() {
		for (size_t i = 0; i + loc < length; i++) {
			arr(i) = arr(loc + i);
		}
		loc = 0;
		length = Size;
	}

public:
	size_t Size = 0;
	Queue() {}
	Queue(size_t n) { arr.Allocate(n); }
	void Push(T n) {
		arr[length] = n;
		++length;
		++Size;
		if (length - Size >= SomeBigSize) {
			Refresh();
		}
	}
	T Pop() {
		--Size;
		return arr(loc++);
	}
	T Top() { return arr(loc); }
	void Clear() {
		length = 0;
		loc = 0;
		Size = 0;
	}
};

// this is implemented temporarily.
//  needs improvement
template <typename T> class Deque {

	Array<T> plus_arr;
	Array<T> minus_arr;

	ll right_loc = 0;
	ll left_loc = -1;
	// the used memory is (left,right)

	T& index(ll loc) { return loc >= 0 ? plus_arr[loc] : minus_arr[-loc - 1]; }

public:
	size_t Size = 0;
	Deque() {}
	void PushRight(const T& val) {
		index(right_loc) = val;
		++right_loc;
		++Size;
	}
	void PushLeft(const T& val) {
		index(left_loc) = val;
		--left_loc;
		++Size;
	}
	T PopRight() {
		--right_loc;
		--Size;
		return index(right_loc);
	}
	T PopLeft() {
		++left_loc;
		--Size;
		return index(left_loc);
	}
	T& LeftTop() { return index(left_loc + 1); }

	T& RightTop() { return index(right_loc - 1); }
};
