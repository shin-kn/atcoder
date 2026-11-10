#pragma once
// # pragma GCC target("avx2")
// # pragma GCC optimize("O3")
// # pragma GCC optimize("unroll-loops")

#include <cassert>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <iostream>
#include <limits>
#include <numbers>
#include <string>
#include <type_traits>
#include <typeinfo>

using std::cin;
using std::cout;
using std::endl;
using std::string;

using ull = uint64_t;
using ll = int64_t;
using ld = long double;

using uint128_t = __uint128_t;
using int128_t = __int128_t;

inline std::ostream& operator<<(std::ostream& os, uint128_t n) {
	char buf[40]; // 2^128 has 39 digits
	char* p = buf + 39;
	*p = '\0';
	do {
		*--p = char('0' + n % 10);
		n /= 10;
	} while (n > 0);
	return os << p;
}

inline std::ostream& operator<<(std::ostream& os, int128_t n) {
	// negate in unsigned so that the minimum value does not overflow
	if (n < 0)
		return os << '-' << (uint128_t(0) - uint128_t(n));
	return os << uint128_t(n);
}

inline std::istream& operator>>(std::istream& is, uint128_t& n) {
	string s;
	is >> s;
	n = 0;
	for (char c : s)
		n = n * 10 + uint128_t(c - '0');
	return is;
}

inline std::istream& operator>>(std::istream& is, int128_t& n) {
	string s;
	is >> s;
	bool neg = s[0] == '-'; // s[0] is '\0' when s is empty
	uint128_t mag = 0;
	for (size_t i = (neg || s[0] == '+'); i < s.size(); ++i)
		mag = mag * 10 + uint128_t(s[i] - '0');
	n = int128_t(neg ? uint128_t(0) - mag : mag);
	return is;
}

using same = std::is_same<size_t, uint64_t>;

static_assert(same::value == true, "size_t == uint64_t");

constexpr ull NiceP = 998244353;
// NMod,mod,nmod
constexpr bool DEBUG = false;

constexpr ull SomeBigSize = 1024 * 1024 * 1;

constexpr ull NMOD_COMB_CACHE_N = 1024;

constexpr ull FOURIER_NO_DFS = 1024;

constexpr bool PrintWithEndl = DEBUG;

template <typename T> constexpr T PI = std::numbers::pi_v<T>;

template <std::integral T> class InftyValue {
public:
	constexpr static T value = std::numeric_limits<T>::max();
};

template <typename T> constexpr inline T Infty = InftyValue<T>::value;

template <typename T>
concept SmallerDefined = requires(T a, T b) {
	{ a < b } -> std::convertible_to<bool>;
};

template <typename T>
concept BiggerDefined = requires(T a, T b) {
	{ a > b } -> std::convertible_to<bool>;
};

template <typename Arr, typename T>
concept ArrayLike = requires(Arr arr, ull index) {
	{ arr.Length } -> std::convertible_to<ull>;
	{ arr[index] } -> std::same_as<T&>;
};

template <typename ArrayType>
using ArrayElementWithoutCheck = std::remove_cvref_t<decltype(std::declval<ArrayType>()[0])>;

template <typename ArrayType>
  requires ArrayLike<ArrayType, ArrayElementWithoutCheck<ArrayType>>
using ArrayElement = ArrayElementWithoutCheck<ArrayType>;
