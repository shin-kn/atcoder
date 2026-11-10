#pragma once
#include "../basic/array.cpp"
#include "../basic/base.cpp"
#include "../basic/light_array.cpp"
#include "../basic/math.cpp"
#include "../basic/min_max.cpp"
#include "../basic/set_dict.cpp"
#include "./integer.cpp"

size_t Dynamic_Mod_P = 100;

class DynamicMod {
public:
	size_t val;
	DynamicMod() { val = 0; }
	DynamicMod(size_t init) : val((init < Dynamic_Mod_P) ? init : init % Dynamic_Mod_P) {}
	friend inline DynamicMod operator+(DynamicMod a, DynamicMod b) {
		return DynamicMod(a.val + b.val);
	}
	friend inline DynamicMod operator-(DynamicMod a, DynamicMod b) {
		return DynamicMod(Dynamic_Mod_P + a.val - b.val);
	}
	friend inline DynamicMod operator*(DynamicMod a, DynamicMod b) {
		return DynamicMod(a.val * b.val);
	}
	size_t Value() { return val; }
	bool operator==(DynamicMod other) { return (val == other.val); }
};

inline std::ostream& operator<<(std::ostream& os, DynamicMod val) {
	os << val.Value();
	return os;
}

struct DirectInit {};

template <size_t P> class Mod { // P should be less than root MAX_SIZE_N

	size_t val;

public:
	constexpr static bool IsMont = (P == 998244353);
	// constexpr static bool IsMont=false;
	constexpr static size_t LogR = (IsMont ? (size_t)30 : 0);
	constexpr static size_t R_dash = 928055296;
	constexpr static size_t P_dash = 998244351;
	constexpr static size_t ModR = ((size_t)1 << LogR) - 1;

	constexpr Mod() = default;

	constexpr Mod(ll n) { this->Set(n >= 0 ? (size_t)n : P - (size_t)(-n) % P); }

	constexpr inline Mod(ull n, DirectInit) : val(n) {}

	constexpr inline static Mod Raw(ull n) { return Mod(n, DirectInit{}); }

	constexpr inline size_t Montgomery(size_t T) {
		size_t res = (((((T & ModR) * P_dash) & ModR) * P + T) >> LogR);
		if (res >= P)
			res -= P;
		return res;
	}

	constexpr inline Mod& Set(size_t init) {
		if constexpr (IsMont) {
			if (init == 0)
				val = init;
			else {
				init = init < P ? init : init % P;
				init <<= LogR;
				val = init % P;
			}
		} else {
			val = init < P ? init : init % P;
		}
		return *this;
	}

	friend constexpr inline Mod operator+(Mod a, Mod b) { return a += b; }
	friend constexpr inline Mod operator-(Mod a, Mod b) { return a -= b; }
	friend constexpr inline Mod operator*(Mod a, Mod b) { return a *= b; }
	friend constexpr inline Mod operator/(Mod a, Mod b) { return a /= b; }

	constexpr inline Mod& operator+=(Mod other) {
		val += other.val;
		if (val >= P)
			val -= P;
		return *(this);
	}
	constexpr inline Mod& operator-=(Mod other) {
		val += P;
		val -= other.val;
		if (val >= P)
			val -= P;
		return *(this);
	}
	constexpr inline Mod& operator*=(Mod other) {
		if constexpr (IsMont) {
			val = Montgomery(val * other.val);
		} else {
			val *= other.val;
			if (val >= P)
				val = val % P;
		}
		return (*this);
	}
	constexpr inline Mod& operator/=(Mod other) {
		if constexpr (IsMont) {
			val = Montgomery(val * (other.Inv().val));
		} else {
			val *= other.Inv().val;
			if (val >= P)
				val = val % P;
		}
		return (*this);
	}
	constexpr inline Mod Inv() {
		assert(val != 0);
		if constexpr (IsMont) {
			return Raw(
			  static_cast<size_t>(
			    static_cast<signed long long int>(P) +
			    ((AxBy(
			      static_cast<signed long long int>(Montgomery(Montgomery(val))),
			      static_cast<signed long long int>(P)
			    ))[0])
			  )
			);
		} else {
			return Raw(
			  static_cast<size_t>(
			    static_cast<signed long long int>(P) +
			    ((AxBy(static_cast<signed long long int>(val), static_cast<signed long long int>(P)))[0])
			  )
			);
		}
	}

	constexpr bool operator==(Mod other) { return val == other.val; }
	constexpr size_t Value() {
		if constexpr (IsMont) {
			return Montgomery(this->val);
		} else {
			return val;
		}
	}
	constexpr Mod& operator=(size_t num) { return Set(num); }
};

template <size_t P> std::ostream& operator<<(std::ostream& os, Mod<P> val) {
	os << val.Value();
	return os;
}

template <ull P> constexpr Mod<P> NthRoot(ull n) {
	static_assert(P == NiceP);
	ull loglength = Log2(n);
	return Power(Mod<P>((ull)15311432), ((size_t)1 << (23 - loglength)));
}

template <ull P, bool Inverse = false> class NthRootsCacheManagerClass {
public:
	inline static Mod<P>* cache[24]{}; // 24 if P=NiceP, we will later extends to other Ps

	Mod<P>* get_cache(ull N) {
		ull log_len = Log2(N);
		if (cache[log_len] != nullptr)
			return cache[log_len];

		Mod<P> zeta = Inverse ? Mod<P>(1) / NthRoot<P>(N) : NthRoot<P>(N);
		ull half_len = N >> 1;
		cache[log_len] = new Mod<P>[half_len];
		Mod<P>* this_cache = cache[log_len];
		this_cache[0] = 1;
		for (ull i = 1; i < half_len; ++i) {
			this_cache[i] = this_cache[i - 1] * zeta;
		}
		return this_cache;
	}
};

NthRootsCacheManagerClass<NiceP> NiceRoots;
NthRootsCacheManagerClass<NiceP, true> NiceInverseRoots;

template <ull P>
void fourier_transform_forward_part(
  Mod<P>* cache, const LightArray<Mod<P>>& arr, ull arr_len, ull start, ull total_deg, ull deg
) {
	const ull Length = 1ull << (total_deg - deg - 1);
	if (deg == total_deg - 1) {
		if (start < arr_len) {
			*cache = arr[start];
		}
		return;
	} else {

		if (Length <= FOURIER_NO_DFS) {

			for (ull i = 0; i < Length; ++i) {
				if (start + i < arr_len) {
					cache[i] = arr[start + i];
				}
			}

			ull half_len = 1;
			ull len = 2;
			ull d = total_deg - 1;
			while (d > deg) {
				--d;
				Mod<P>* zetas_cache = NiceRoots.get_cache(len);
				for (ull i = 0; i < half_len; ++i) {
					Mod<P>* cache_1 = cache + i;
					Mod<P>* cache_2 = cache + half_len + i;
					for (ull j = 0; j < (ull)1 << (d - deg); ++j) {
						Mod<P> even = *cache_1;
						Mod<P> odd = (*zetas_cache) * (*cache_2);
						*cache_1 = even + odd;
						*cache_2 = even - odd;
						cache_1 += len;
						cache_2 += len;
					}
					++zetas_cache;
				}
				half_len <<= 1;
				len <<= 1;
			}
		} else {
			const ull half_len = 1ull << (total_deg - deg - 2);
			fourier_transform_forward_part<P>(cache, arr, arr_len, start, total_deg, deg + 1);
			fourier_transform_forward_part<P>(
			  cache + half_len, arr, arr_len, start + half_len, total_deg, deg + 1
			);

			Mod<P>* zetas_cache = NiceRoots.get_cache(half_len << 1);
			for (ull i = 0; i < half_len; ++i) {
				Mod<P> even = cache[i];
				Mod<P> odd = (*zetas_cache) * cache[half_len + i];
				cache[i] = even + odd;
				cache[half_len + i] = even - odd;
				++zetas_cache;
			}
		}
	}
}

template <ull P>
LightArray<Mod<P>>
fourier_transform_forward(const LightArray<Mod<P>>& arr, ull arr_len, ull Length) {
	static_assert(P == NiceP);
	assert(Length >= 1 && (ull)1 << Log2(Length) == Length);
	ull total_deg = Log2(Length) + 1;
	Mod<P>* cache = new Mod<P>[Length]();
	arr_len = Min(arr_len, Length);

	fourier_transform_forward_part<P>(cache, arr, arr_len, 0, total_deg, 0);
	return LightArray(cache, Length);
}

template <ull P>
void fourier_transform_inverse_part(Mod<P>* cache_1, Mod<P>* cache_2, ull total_deg, ull deg) {
	const ull Length = 1ull << (total_deg - deg - 1);
	if (deg == total_deg - 1) {
		return;
	} else {

		if (Length <= FOURIER_NO_DFS) {

			ull half_len = 1ull << (total_deg - deg - 2);
			ull len = Length;
			for (ull d = deg; d < total_deg - 1; ++d) {
				Mod<P>* inv_zetas_cache = NiceInverseRoots.get_cache(half_len << 1);
				for (ull i = 0; i < half_len; ++i) {
					Mod<P>* cache_1a = cache_1 + i;
					Mod<P>* cache_1b = cache_1 + half_len + i;
					Mod<P>* cache_2a = cache_2 + i;
					Mod<P>* cache_2b = cache_2 + half_len + i;
					for (ull j = 0; j < (ull)1 << (d - deg); ++j) {
						Mod<P> even_1 = *cache_1a;
						Mod<P> odd_1 = *cache_1b;
						*cache_1a = (even_1 + odd_1);
						*cache_1b = (even_1 - odd_1) * (*inv_zetas_cache);

						Mod<P> even_2 = *cache_2a;
						Mod<P> odd_2 = *cache_2b;
						*cache_2a = (even_2 + odd_2);
						*cache_2b = (even_2 - odd_2) * (*inv_zetas_cache);

						cache_1a += len;
						cache_1b += len;
						cache_2a += len;
						cache_2b += len;
					}
					++inv_zetas_cache;
				}
				half_len >>= 1;
				len >>= 1;
			}
		} else {
			const ull half_len = 1ull << (total_deg - deg - 2);
			Mod<P>* inv_zetas_cache = NiceInverseRoots.get_cache(half_len << 1);
			for (ull i = 0; i < half_len; ++i) {
				Mod<P> even_1 = cache_1[i];
				Mod<P> odd_1 = cache_1[half_len + i];
				cache_1[i] = (even_1 + odd_1);
				cache_1[half_len + i] = (even_1 - odd_1) * (*inv_zetas_cache);

				Mod<P> even_2 = cache_2[i];
				Mod<P> odd_2 = cache_2[half_len + i];
				cache_2[i] = (even_2 + odd_2);
				cache_2[half_len + i] = (even_2 - odd_2) * (*inv_zetas_cache);

				++inv_zetas_cache;
			}
			fourier_transform_inverse_part<P>(cache_1, cache_2, total_deg, deg + 1);
			fourier_transform_inverse_part<P>(cache_1 + half_len, cache_2 + half_len, total_deg, deg + 1);
		}
	}
}

template <ull P>
void fourier_transform_inverse(
  const LightArray<Mod<P>>& arr_1,
  const LightArray<Mod<P>>& arr_2,
  LightArray<Mod<P>>& res_1,
  LightArray<Mod<P>>& res_2,
  ull arr_len_1,
  ull arr_len_2,
  ull Length
) {
	static_assert(P == NiceP);
	assert(Length >= 1 && (ull)1 << Log2(Length) == Length);
	ull total_deg = Log2(Length) + 1;
	Mod<P>* cache_1 = new Mod<P>[Length]();
	Mod<P>* cache_2 = new Mod<P>[Length]();
	arr_len_1 = Min(arr_len_1, Length);
	arr_len_2 = Min(arr_len_2, Length);
	for (ull i = 0; i < arr_len_1; ++i) {
		cache_1[i] = arr_1[i];
	}
	for (ull i = 0; i < arr_len_2; ++i) {
		cache_2[i] = arr_2[i];
	}
	fourier_transform_inverse_part<P>(cache_1, cache_2, total_deg, 0);
	res_1 = LightArray(cache_1, Length);
	res_2 = LightArray(cache_2, Length);
}

using NMod = Mod<NiceP>;

template <typename T> inline T Factorial(ull n) {
	T counter(1);
	for (ull i = 1; i <= n; ++i) {
		counter *= T(i);
	}
	return counter;
}
template <> inline NMod Factorial<NMod>(ull n) {
	static LightArray<NMod> arr(NMOD_COMB_CACHE_N, 1);
	static ull cached = 1;

	for (; cached < NMOD_COMB_CACHE_N; ++cached) {
		arr[cached] = arr[cached - 1] * cached;
	}
	if (n >= NMOD_COMB_CACHE_N) {
		NMod val = arr[NMOD_COMB_CACHE_N - 1];
		for (ull i = NMOD_COMB_CACHE_N; i <= n; ++i) {
			val *= i;
		}
		return val;
	}
	return arr[n];
}

template <typename T> inline T Comb(ull n, ull m) {
	assert(n > 0 && m >= 0);
	static Dict<Tuple<ull, 2>, T> dict;
	if (dict.Has(Tuple(n, m)))
		return dict[Tuple(n, m)];

	T counter(1);
	for (T i = 1; i <= m; ++i) {
		counter *= T(n - i + 1);
		counter /= T(i);
	}
	dict[Tuple(n, m)] = counter;
	return counter;
}

template <> inline NMod Comb<NMod>(ull n, ull m) {

	static Dict<Tuple<ull, 2>, NMod> dict;
	if (n <= NMOD_COMB_CACHE_N) {

		if (dict.Has(Tuple(n, m)))
			return dict[Tuple(n, m)];
		NMod res = Factorial<NMod>(n) / Factorial<NMod>(m) / Factorial<NMod>(n - m);
		dict[Tuple(n, m)] = res;
		return res;
	}

	NMod res = Factorial<NMod>(n) / Factorial<NMod>(m) / Factorial<NMod>(n - m);
	return res;
}
