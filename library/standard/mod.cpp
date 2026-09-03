#pragma once
#include "../basic.cpp"

size_t Dynamic_Mod_P = 100;

class DynamicMod {
public:
	size_t val;
	DynamicMod() { val = 0; }
	DynamicMod(size_t init)
	    : val((init < Dynamic_Mod_P) ? init : init % Dynamic_Mod_P) {}
	inline DynamicMod operator+(DynamicMod other) {
		return DynamicMod(val + other.val);
	}
	inline DynamicMod operator-(DynamicMod other) {
		return DynamicMod(Dynamic_Mod_P + val - other.val);
	}
	inline DynamicMod operator*(DynamicMod other) {
		return DynamicMod(val * other.val);
	}
	size_t Value() { return val; }
	bool operator==(DynamicMod other) { return (val == other.val); }
	DynamicMod& operator=(DynamicMod other) {
		val = other.val;
		return *(this);
	}
};

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

	Mod() { val = 0; }

	Mod(ull n) { this->Set(n); }
	inline Mod(ull n, DirectInit) : val(n) {}

	inline static Mod Raw(ull n) { return Mod(n, DirectInit{}); }

	inline size_t Montgomery(size_t T) {
		size_t res = (((((T & ModR) * P_dash) & ModR) * P + T) >> LogR);
		if (res >= P)
			res -= P;
		return res;
	}

	inline Mod& Set(size_t init) {
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

	inline Mod operator+(Mod other) {
		size_t res = val + other.val;
		if (res > P) {
			return Raw(res - P);
		}
		return Raw(res);
	}
	inline Mod& operator+=(Mod other) {
		val += other.val;
		if (val >= P)
			val -= P;
		return *(this);
	}
	inline Mod operator-(Mod other) {
		size_t res = P + val - other.val;
		if (res >= P) {
			return Raw(res - P);
		}
		return Raw(res);
	}
	inline Mod& operator-=(Mod other) {
		val += P;
		val -= other.val;
		if (val >= P)
			val -= P;
		return *(this);
	}
	inline Mod operator*(Mod other) {
		if constexpr (IsMont) {
			return Raw(Montgomery(val * other.val));
		} else {
			size_t res = val * other.val;
			if (res >= P)
				res = res % P;
			return Raw(res);
		}
	}
	inline Mod operator*(ull other) { return *this * Mod().Set(other); }

	inline Mod operator/(Mod other) { return (*this) * other.Inv(); }

	inline Mod& operator*=(Mod other) {
		if constexpr (IsMont) {
			val = Montgomery(val * other.val);
		} else {
			val *= other.val;
			if (val >= P)
				val = val % P;
		}
		return (*this);
	}
	inline Mod& operator/=(Mod other) {
		if constexpr (IsMont) {
			val = Montgomery(val * (other.Inv().val));
		} else {
			val *= other.Inv().val;
			if (val >= P)
				val = val % P;
		}
		return (*this);
	}
	inline Mod Inv() {
		if (val == 0)
			std::exit(EXIT_FAILURE);
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
			    ((AxBy(
			      static_cast<signed long long int>(val),
			      static_cast<signed long long int>(P)
			    ))[0])
			  )
			);
		}
	}

	bool operator==(Mod other) { return val == other.val; }
	size_t Value() {
		if constexpr (IsMont) {
			return Montgomery(this->val);
		} else {
			return val;
		}
	}
	Mod& operator=(size_t num) { return Set(num); }
	Mod& operator=(Mod other) {
		val = other.val;
		return *this;
	}
};

template <size_t P> std::ostream& operator<<(std::ostream& os, Mod<P> val) {
	os << val.Value();
	return os;
}

template <ull P, ArrayLike<Mod<P>> ArrayType>
inline LightArray<Mod<P>> fourier_transform_forward(
  ArrayType& arr, ull length, Mod<P> zeta, ull arr_len
) {
	assert(length >= 1 && (ull)1 << Log2(length) == length);
	ull total_deg = Log2(length) + 1;
	Mod<P>* cache = new Mod<P>[length]();
	Mod<P>* zeta_cache = new Mod<P>[length / 2];
	zeta_cache[0] = Mod<P>(1);
	for (ull i = 1; i < length / 2; ++i)
		zeta_cache[i] = zeta_cache[i - 1] * zeta;

	auto dfs = [&](auto& self, ull deg, ull start) -> void {
		Mod<P>* this_cache = cache + start;
		if (deg == total_deg - 1) {
			if (start < arr_len) {
				*this_cache = arr[start];
			}
			return;
		}

		ull half_len = 1ull << (total_deg - 1 - deg - 1);
		self(self, deg + 1, start);
		self(self, deg + 1, start + half_len);

		Mod<P>* this_zeta_cache = zeta_cache;
		for (ull i = 0; i < half_len; ++i) {
			Mod<P> even = this_cache[i];
			Mod<P> odd = (*this_zeta_cache) * this_cache[half_len + i];
			this_cache[i] = even + odd;
			this_cache[half_len + i] = even - odd;
			this_zeta_cache += (ull)1 << deg;
		}
	};

	dfs(dfs, 0, 0);

	delete[] zeta_cache;
	return LightArray(cache, length);
}

template <ull P, ArrayLike<Mod<P>> ArrayType>
void fourier_transform_inverse(
  ArrayType& arr_1,
  ArrayType& arr_2,
  LightArray<Mod<P>>& res_1,
  LightArray<Mod<P>>& res_2,
  ull length,
  Mod<P> zeta,
  ull arr_len_1,
  ull arr_len_2
) {
	assert(length >= 1 && (ull)1 << Log2(length) == length);
	ull total_deg = Log2(length) + 1;
	Mod<P>* cache_1 = new Mod<P>[length]();
	Mod<P>* cache_2 = new Mod<P>[length]();
	for (ull i = 0; i < arr_len_1; ++i) {
		cache_1[i] = arr_1[i];
	}

	for (ull i = 0; i < arr_len_2; ++i) {
		cache_2[i] = arr_2[i];
	}
	Mod<P>* invs_cache = new Mod<P>[length / 2];
	invs_cache[0] = Mod<P>(1);
	Mod<P> inv_zeta = Mod<P>(1) / zeta;
	for (ull i = 1; i < length / 2; ++i) {
		invs_cache[i] = invs_cache[i - 1] * inv_zeta;
	}

	auto dfs = [&](auto& self, ull deg, ull start) -> void {
		if (deg == total_deg - 1) {
			return;
		}

		ull half_len = 1ull << (total_deg - 1 - deg - 1);

		Mod<P>* this_cache_1 = cache_1 + start;
		Mod<P>* this_cache_2 = cache_2 + start;

		Mod<P>* this_invs_cache = invs_cache;
		for (ull i = 0; i < half_len; ++i) {
			Mod<P> even_1 = this_cache_1[i];
			Mod<P> odd_1 = this_cache_1[half_len + i];
			this_cache_1[i] = (even_1 + odd_1);
			this_cache_1[half_len + i] = (even_1 - odd_1) * (*this_invs_cache);

			Mod<P> even_2 = this_cache_2[i];
			Mod<P> odd_2 = this_cache_2[half_len + i];
			this_cache_2[i] = (even_2 + odd_2);
			this_cache_2[half_len + i] = (even_2 - odd_2) * (*this_invs_cache);

			this_invs_cache += (ull)1 << deg;
		}

		// inv_zeta at depth deg+1 is inv_zeta^2, matching zeta^2
		self(self, deg + 1, start);
		self(self, deg + 1, start + half_len);
	};
	dfs(dfs, 0, 0);

	res_1 = LightArray(cache_1, length);
	res_2 = LightArray(cache_2, length);

	delete[] invs_cache;
}

template <size_t P>
inline LightArray<Mod<P>>
fouriertransform_freq(Array<Mod<P>>& arr, size_t degree, Mod<P> zeta) {
	size_t length = 1 << degree;

	Mod<P>** res = new (std::nothrow) Mod<P>*[degree + 1];
	if (res == nullptr)
		std::exit(EXIT_FAILURE);
	for (size_t i = 1; i <= degree; ++i) {
		res[i] = new (std::nothrow) Mod<P>[length];
		if (res[i] == nullptr)
			std::exit(EXIT_FAILURE);
	}

	Mod<P> tempzeta;
	tempzeta = 1;
	LightArray<Mod<P>> cache;
	cache.Allocate(length >> 1);
	{
		size_t loc1 = 0;
		size_t loc2 = length >> 1;

		while (loc2 < length) {
			res[1][loc1] = arr[loc1] + arr[loc2];
			res[1][loc2] = tempzeta * (arr[loc1] - arr[loc2]);
			cache[loc1] = tempzeta;
			tempzeta *= zeta;
			++loc2;
			++loc1;
		}
	}

	for (size_t deg = 2; deg < degree; ++deg) {
		size_t looplength = (size_t)1 << (degree - deg);
		size_t loopnum = (size_t)1 << (deg - 1);
		size_t loc1 = 0;
		size_t loc2 = looplength;
		size_t cache_dif = loopnum;
		Mod<P>* dist = res[deg];
		Mod<P>* olddist = res[deg - 1];
		for (size_t i = 0; i < loopnum; ++i) {
			size_t cache_loc = 0;
			for (size_t j = 0; j < looplength; ++j) {
				dist[loc1] = olddist[loc1] + olddist[loc2];
				dist[loc2] = cache[cache_loc] * (olddist[loc1] - olddist[loc2]);
				cache_loc += cache_dif;
				++loc1;
				++loc2;
			}
			loc1 = loc2;
			loc2 = loc1 + looplength;
		}
	}

	{
		size_t deg = degree;
		size_t looplength = (size_t)1 << (degree - deg);
		size_t loopnum = (size_t)1 << (deg - 1);
		size_t loc1 = 0;
		size_t loc2 = looplength;
		Mod<P>* dist = res[deg];
		Mod<P>* olddist = res[deg - 1];
		for (size_t i = 0; i < loopnum; ++i) {
			for (size_t j = 0; j < looplength; ++j) {
				dist[loc1] = olddist[loc1] + olddist[loc2];
				dist[loc2] = (olddist[loc1] - olddist[loc2]);
				++loc1;
				++loc2;
			}
			loc1 = loc2;
			loc2 = loc1 + looplength;
		}
	}
	Mod<P>* result = res[degree];
	for (size_t i = 1; i < degree; ++i) {
		delete[] res[i];
	}
	delete[] res;
	return LightArray<Mod<P>>(result, length);
}

template <size_t P>
inline LightArray<Mod<P>>
fouriertransform_time(Array<Mod<P>>& arr, size_t degree, Mod<P> zeta) {
	size_t length = 1 << degree;

	Mod<P>** res = new (std::nothrow) Mod<P>*[degree + 1];
	if (res == nullptr)
		std::exit(EXIT_FAILURE);
	for (size_t i = 1; i <= degree; ++i) {
		res[i] = new (std::nothrow) Mod<P>[length];
		if (res[i] == nullptr)
			std::exit(EXIT_FAILURE);
	}

	Mod<P> tempzeta;
	tempzeta = 1;
	LightArray<Mod<P>> cache;
	cache.Allocate(length >> 1);
	{
		size_t looplength = length >> 1;
		for (size_t i = 0; i < looplength; ++i) {
			cache[i] = tempzeta;
			tempzeta *= zeta;
		}
	}
	{
		size_t deg = 1;
		size_t looplength = (size_t)1 << (deg - 1);
		size_t loopnum = (size_t)1 << (degree - deg - 1);
		size_t loc1 = 0;
		size_t loc2 = looplength;
		size_t cache_dif = (size_t)1 << (degree - deg - 1);
		Mod<P>* dist = res[deg];
		Array<Mod<P>>& olddist = arr;
		for (size_t i = 0; i < loopnum; ++i) {
			for (size_t j = 0; j < looplength; ++j) {
				dist[loc1] = olddist[loc1] + olddist[loc2];
				dist[loc2] = (olddist[loc1] - olddist[loc2]);
				++loc1;
				++loc2;
			}
			size_t cache_loc_1 = 0;
			size_t cache_loc_2 = length >> 2;

			loc1 = loc2;
			loc2 = loc1 + looplength;
			for (size_t j = 0; j < looplength; ++j) {
				dist[loc1] = cache[cache_loc_1] * (olddist[loc1] + olddist[loc2]);
				dist[loc2] = cache[cache_loc_2] * (olddist[loc1] - olddist[loc2]);
				++loc1;
				++loc2;
				cache_loc_1 += cache_dif;
				cache_loc_2 += cache_dif;
			}

			loc1 = loc2;
			loc2 = loc1 + looplength;
		}
	}

	for (size_t deg = 2; deg < degree; ++deg) {
		size_t looplength = (size_t)1 << (deg - 1);
		size_t loopnum = (size_t)1 << (degree - deg - 1);
		size_t loc1 = 0;
		size_t loc2 = looplength;
		size_t cache_dif = (size_t)1 << (degree - deg - 1);
		Mod<P>* dist = res[deg];
		Mod<P>* olddist = res[deg - 1];
		for (size_t i = 0; i < loopnum; ++i) {
			for (size_t j = 0; j < looplength; ++j) {
				dist[loc1] = olddist[loc1] + olddist[loc2];
				dist[loc2] = (olddist[loc1] - olddist[loc2]);
				++loc1;
				++loc2;
			}
			size_t cache_loc_1 = 0;
			size_t cache_loc_2 = length >> 2;

			loc1 = loc2;
			loc2 = loc1 + looplength;
			for (size_t j = 0; j < looplength; ++j) {
				dist[loc1] = cache[cache_loc_1] * (olddist[loc1] + olddist[loc2]);
				dist[loc2] = cache[cache_loc_2] * (olddist[loc1] - olddist[loc2]);
				++loc1;
				++loc2;
				cache_loc_1 += cache_dif;
				cache_loc_2 += cache_dif;
			}

			loc1 = loc2;
			loc2 = loc1 + looplength;
		}
	}
	{
		size_t deg = degree;
		size_t loc1 = 0;
		size_t loc2 = length >> 1;
		Mod<P>* dist = res[deg];
		Mod<P>* olddist = res[deg - 1];
		size_t looplength = length >> 1;
		for (size_t j = 0; j < looplength; ++j) {
			dist[loc1] = olddist[loc1] + olddist[loc2];
			dist[loc2] = (olddist[loc1] - olddist[loc2]);
			++loc1;
			++loc2;
		}
	}

	Mod<P>* result = res[degree];
	for (size_t i = 1; i < degree; ++i) {
		delete[] res[i];
	}
	delete[] res;
	return LightArray<Mod<P>>(result, length);
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
	static Array<NMod> arr(NMOD_COMB_CACHE_N);
	if (arr.Length <= n) {
		for (ull i = arr.Length; i <= n; ++i) {
			if (i == 0)
				arr[i] = 1;
			else {
				arr[i] = arr[i - 1] * NMod(i);
			}
		}
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
