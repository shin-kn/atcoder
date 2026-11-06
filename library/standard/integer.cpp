#pragma once
#include "../basic/array.cpp"
#include "../basic/base.cpp"
#include "../basic/tuple_pair.cpp"
#include "../basic/utils.cpp"
#include "search.cpp"

template <std::integral T> inline T Sqrt(T num) { // assume num>=0
	// res*res<=num is asserted
	// upper keeps Sup's lower+upper from overflowing; n<=num/n replaces n*n<=num
	const T cap = T(1) << ((std::numeric_limits<T>::digits + 1) / 2);
	const T upper = num < cap ? num : cap;
	return Sup(T(0), upper, [=](T n) -> bool { return n <= num / n; });
}

template <typename T> constexpr inline Tuple<T, 2> AxBy(T a, T b) { // a!=0,b!=0
	if (a == 0 || b == 0)
		return Tuple<T, 2>(0, 0);
	if (Abs(a) < Abs(b)) {
		T rem = b % a;
		if (rem == 0) {
			return Tuple<T, 2>(Abs(a) / a, 0);
		}
		Tuple<T, 2> tuple = AxBy(a, rem);
		tuple[0] = tuple[0] - tuple[1] * ((b - rem) / a);
		return tuple;
	}
	T rem = a % b;
	if (rem == 0) {
		return Tuple<T, 2>(0, Abs(b) / b);
	}
	Tuple<T, 2> tuple = AxBy(rem, b);
	tuple[1] = tuple[1] - tuple[0] * ((a - rem) / b);
	return tuple;
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

template <typename T> inline constexpr bool IsPrimeNaive(T n) {
	for (T i = 2; i * i <= n; ++i) {
		if (n % i == 0)
			return false;
	}
	return true;
}

class Eratosthenes {                       // N log^2 N
	Array<bool> arr = Array<bool>(2, false); // arr.length=Length+1
	ull Length = 1;

public:
	Array<size_t> PrimeNumbers;

	void Allocate(size_t n) {
		if (n <= Length)
			return;

		arr.Allocate(n + 1);
		for (ull i = Length + 1; i < n + 1; ++i)
			arr[i] = true;

		for (auto&& prime : PrimeNumbers) {
			ull loc = (Length + prime) / prime * prime;
			while (loc <= n) {
				arr[loc] = false;
				loc += prime;
			}
		}

		for (ull i = Length + 1; i < n + 1; ++i) {
			if (!arr[i])
				continue;
			PrimeNumbers.Push(i);
			ull loc = i * 2;
			while (loc <= n) {
				arr[loc] = false;
				loc += i;
			}
		}
		Length = n;
	}
	Eratosthenes() {}
	Eratosthenes(size_t n) { Allocate(n); }
	inline bool operator[](size_t ind) { return arr[ind]; }
	bool IsPrime(ull n) { return arr[n]; }
};

Array<ull> Factorize(ull n) {
	static Eratosthenes erat;
	assert(n != 0);
	erat.Allocate(Sqrt(n));

	Array<ull> res;

	for (auto&& prime : erat.PrimeNumbers) {
		if (prime * prime > n) {
			break;
		}
		while (n % prime == 0) {
			res.Push(prime);
			n /= prime;
		}
	}
	if (n != 1)
		res.Push(n);
	return res;
}

class Fraction {
	// reduces n/d with int128_t and stores it as ll
	void Set(int128_t n, int128_t d);

public:
	ll num = 0;
	ll den = 1; // den>0, gcd(num,den)==1
	Fraction() {}
	Fraction(ll n) : num(n) {}
	Fraction(ll n, ll d) { Set(n, d); }
	bool operator==(const Fraction& other) const {
		return num == other.num && den == other.den;
	}
	bool operator<(const Fraction& other) const {
		return int128_t(num) * other.den < int128_t(other.num) * den;
	}
	bool operator>(const Fraction& other) const { return other < *this; }
	Fraction operator+(const Fraction& other) const;
	Fraction operator-(const Fraction& other) const;
	Fraction operator*(const Fraction& other) const;
	Fraction Inv() const;
	Fraction operator/(const Fraction& other) const {
		return *this * other.Inv();
	}
};

inline void Fraction::Set(int128_t n, int128_t d) {
	assert(d != 0);
	if (d < 0) {
		d = -d;
		n = -n;
	}
	int128_t a = n < 0 ? -n : n;
	int128_t b = d;
	while (b != 0) {
		int128_t r = a % b;
		a = b;
		b = r;
	}
	num = ll(n / a);
	den = ll(d / a);
}

inline Fraction Fraction::operator+(const Fraction& other) const {
	Fraction res;
	res.Set(
	  int128_t(num) * other.den + int128_t(other.num) * den,
	  int128_t(den) * other.den
	);
	return res;
}

inline Fraction Fraction::operator-(const Fraction& other) const {
	Fraction res;
	res.Set(
	  int128_t(num) * other.den - int128_t(other.num) * den,
	  int128_t(den) * other.den
	);
	return res;
}

inline Fraction Fraction::operator*(const Fraction& other) const {
	Fraction res;
	res.Set(int128_t(num) * other.num, int128_t(den) * other.den);
	return res;
}

inline Fraction Fraction::Inv() const {
	assert(num != 0);
	return Fraction(den, num);
}

inline std::ostream& operator<<(std::ostream& os, Fraction val) {
	os << val.num << "/" << val.den;
	return os;
}
