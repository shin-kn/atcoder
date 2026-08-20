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

using same = std::is_same<size_t, uint64_t>;

static_assert(same::value == true, "size_t == uint64_t");

constexpr ull NiceP = 998244353;
// NMod,mod,nmod

constexpr ull SomeBigSize = 1024 * 1024 * 1;

constexpr ull NMOD_COMB_CACHE_N = 1024;

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

template <typename T> struct TypeStruct {};

template <typename T> TypeStruct<T> Type;

template <typename T> using TypeVar = TypeStruct<T>&;

template <typename Arr, typename T>
concept ArrayLike = requires(Arr arr, ull index) {
	{ arr.Length } -> std::convertible_to<ull>;
	{ arr[index] } -> std::same_as<T&>;
};
