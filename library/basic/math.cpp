#pragma once
#include "base.cpp"

constexpr inline size_t BiggerPower2(size_t num) {
	if (num == 0)
		return 1;
	int dig = 64 - std::countl_zero(num) - 1;
	if (((size_t)1 << dig) == num)
		return num;
	return ((size_t)1 << (dig + 1));
}

constexpr inline size_t SmallerPower2(size_t num) { // num must not be 0
	if (num == 0)
		return 0;
	int dig = 64 - std::countl_zero(num) - 1;
	return ((size_t)1 << dig);
}

template <typename T> constexpr inline T Log2(T num) { //
	size_t counter = 1;
	size_t i = 0;
	for (; true; i++) {
		if (counter >= num)
			break;
		counter = counter << 1;
	}
	return i;
}

template <> constexpr inline size_t Log2<size_t>(size_t num) {
	return static_cast<size_t>(64 - std::countl_zero(num) - 1);
}

template <typename T> constexpr T Power(T val, size_t times) {
	T res = T((ull)1);
	while (times > 0) {
		if (times & 1)
			res = res * val;
		times >>= 1;
		// skip the last squaring: it is unused and may overflow
		if (times > 0)
			val = val * val;
	}
	return res;
}
