#pragma once
//# pragma GCC target("avx2")
//# pragma GCC optimize("O3")
//# pragma GCC optimize("unroll-loops")



#include <string>
#include <cmath>
#include <numbers>
#include <typeinfo>
#include <type_traits>
#include <iostream>
#include <cassert>
#include <cstdint>
#include <concepts>


using std::cout;
using std::endl;
using std::cin;
using std::string;



using ull=uint64_t;
using ll=int64_t;
using ld=long double;

using uint128_t =__uint128_t;
using int128_t = __int128_t;


using same = std::is_same<size_t,uint64_t>;

static_assert(same::value == true, "size_t == uint64_t");




constexpr ull NiceP=998244353;
//NMod,mod,nmod


constexpr ull SomeBigSize=1024*1024*1;


template <std::integral T>
class OneValue{
    public:
        constexpr static T value=static_cast<T>((int)1);
};

template <typename T>
constexpr inline T One=OneValue<T>::value;



template <typename T>
constexpr T PI=std::numbers::pi_v<T>;

template <std::unsigned_integral T>
class InftyValue{
    public:
        constexpr static T value=static_cast<T>(0)-One<T>;
};

template <typename T>
constexpr inline T Infty=InftyValue<T>::value;


template<typename T>
concept SmallerDefined = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
};

template<typename T>
concept BiggerDefined = requires(T a, T b) {
    { a > b } -> std::convertible_to<bool>;
};

