#pragma once
#include "array.cpp"
#include "base.cpp"

template <typename T> inline T Min(T val1, T val2) {
	if (val1 < val2)
		return val1;
	return val2;
}

template <typename T> inline T Max(T val1, T val2) {
	if (val1 > val2)
		return val1;
	return val2;
}

template <typename T> inline void UpdateMin(T& val1, T val2) {
	static_assert(std::is_copy_assignable<T>::value);
	if (val2 < val1)
		val1 = val2;
	return;
}
template <typename T> inline void UpdateMax(T& val1, T val2) {
	static_assert(std::is_copy_assignable<T>::value);
	if (val2 > val1)
		val1 = val2;
	return;
}

template <typename ArrayType>
  requires ArrayLike<ArrayType, ArrayElement<ArrayType>>
ArrayElement<ArrayType> Max(ArrayType& arr) {
	if (arr.Length == 0)
		return ArrayElement<ArrayType>();
	ArrayElement<ArrayType> max = arr[0];
	for (size_t i = 1; i < arr.Length; ++i) {
		if (max < arr[i])
			max = arr[i];
	}
	return max;
}

template <typename ArrayType>
  requires ArrayLike<ArrayType, ArrayElement<ArrayType>>
ArrayElement<ArrayType> Min(ArrayType& arr) {
	if (arr.Length == 0)
		return ArrayElement<ArrayType>();
	ArrayElement<ArrayType> min = arr[0];
	for (size_t i = 1; i < arr.Length; ++i) {
		if (min > arr[i])
			min = arr[i];
	}
	return min;
}
