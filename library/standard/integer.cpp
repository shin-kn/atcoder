#pragma once
#include "../basic.cpp"
#include "search.cpp"

template <std::integral T> inline T Sqrt(T num) { // assume num>=0
	// res*res<=num is asserted
	return Sup(T(0), num, [=](T n) -> bool { return n * n <= num; });
}

template <typename T> inline T Combination(T n, T c) {
	if (c >= n / 2 + 1)
		return Combination(n, n - c);
	T counter = 1;
	for (size_t i = 1; i <= c; ++i) {
		counter = counter * (n - i + 1) / i;
	}
	return counter;
}

template <typename T> inline Set<T, 2> AxBy(T a, T b) { // a!=0,b!=0
	if (a == 0 || b == 0)
		return Set<T, 2>(0, 0);
	if (Abs(a) < Abs(b)) {
		T rem = b % a;
		if (rem == 0) {
			return Set<T, 2>(Abs(a) / a, 0);
		}
		Set<T, 2> set = AxBy(a, rem);
		set[0] = set[0] - set[1] * ((b - rem) / a);
		return set;
	}
	T rem = a % b;
	if (rem == 0) {
		return Set<T, 2>(0, Abs(b) / b);
	}
	Set<T, 2> set = AxBy(rem, b);
	set[1] = set[1] - set[0] * ((a - rem) / b);
	return set;
}

template <typename T> inline T GCD(T a, T b) {
	while (a != 0 && b != 0) {
		if (a > b)
			Swap(a, b);
		b = b % a;
	}
	return Abs(a != 0 ? a : b);
}

template <typename T> inline T LCM(T a, T b) { return a * b / GCD(a, b); }

template <typename T> inline constexpr bool IsPrime(T n) {
	for (T i = 2; i * i <= n; ++i) {
		if (n % i == 0)
			return false;
	}
	return true;
}

class Eratosthenes { // N log^2 N
	Array<bool> arr;

public:
	Array<size_t> PrimeNumbers;
	void Init(size_t n) {
		arr.Allocate(n + 1);
		arr.Set(0, n + 1, true);
		arr[1] = false;
		for (size_t i = 2; i <= n; ++i) {
			if (!arr[i])
				continue;
			PrimeNumbers.Push(i);
			for (size_t j = i * 2; j <= n; j += i) {
				arr[j] = false;
			}
		}
	}
	Eratosthenes() {}
	Eratosthenes(size_t n) { Init(n); }
	inline bool operator[](size_t ind) { return arr[ind]; }
};

class Factorizer {
	Eratosthenes erat;

public:
	void Init(size_t n) { erat.Init(n); }
	Factorizer() {}
	Factorizer(size_t n) { Init(n); }

	Array<Set<size_t, 2>> operator()(size_t num) {
		Array<Set<size_t, 2>> arr;
		if (num == 0)
			return arr;
		for (size_t i = 0; num != 1; ++i) {
			size_t j = erat.PrimeNumbers[i];
			size_t counter = 0;
			while (num % j == 0) {
				num /= j;
				++counter;
			}
			if (counter != 0)
				arr.Push(Set<size_t, 2>(j, counter));
		}
		return arr;
	}
};
