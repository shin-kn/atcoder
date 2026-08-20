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
	static Array<NMod> arr;
	if (arr.Length <= n) {
		for (ull i = arr.Length; i <= n; ++i) {
			if (i == 0)
				arr[i].Set(1);
			else {
				arr[i] = arr[i - 1] * NMod(i);
			}
		}
	}
	return arr[n];
}

template <typename T> inline T Comb(ull n, ull m) {
	assert(n > 0 && m >= 0);
	T counter(1);
	for (T i = 1; i <= m; ++i) {
		counter *= T(n - i + 1);
		counter /= T(i);
	}
	return counter;
}

template <> inline NMod Comb<NMod>(ull n, ull m) {
	static NMod cache[NMOD_COMB_CACHE_N]
	                 [NMOD_COMB_CACHE_N + 1]; // some space for improvement
	static bool cache_init[NMOD_COMB_CACHE_N][NMOD_COMB_CACHE_N + 1] = {
	  false
	}; // some space for improvement
	if (n <= NMOD_COMB_CACHE_N) {
		if (cache_init[n][m]) {
			return cache[n][m];
		}
		cache_init[n][m] = true;
		cache[n][m] =
		  Factorial<NMod>(n) / Factorial<NMod>(m) / Factorial<NMod>(n - m);
		return cache[n][m];
	}
	return Factorial<NMod>(n) / Factorial<NMod>(m) / Factorial<NMod>(n - m);
}
