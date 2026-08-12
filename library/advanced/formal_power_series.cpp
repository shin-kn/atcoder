#pragma once
#include "../standard.cpp"

template <typename T> class FormalPowerSeries;

// i<=Degree => arr[i] must be initialized by user

template <size_t P> class FormalPowerSeries<Mod<P>> {
public:
	Array<Mod<P>> arr;
	size_t Degree;
	FormalPowerSeries() : Degree(0) {};
	FormalPowerSeries(size_t N) : Degree(N) { arr.Allocate(N + 1); };

	void Init() { arr.Set(0, Degree + 1, Mod<P>().Set(0)); }
	void Init(Mod<P> val) { arr.Set(0, Degree + 1, val); }

	void SetDegree(size_t deg) {
		for (size_t i = Degree + 1; i <= deg; ++i) {
			arr[i].Set(0);
		}
		Degree = deg;
	}

	FormalPowerSeries operator*(
	FormalPowerSeries& other) { // convolution of Degree(deg)
		size_t deg = Max(Degree, other.Degree);
		size_t length = BiggerPower2(deg * 2 + 1);

		if (P == 998244353 && length <= (size_t)1 << 23) {
			length = Max(length, (size_t)4); // degree>=2
			arr.Set(Degree + 1, length, Mod<P>().Set(0));
			other.arr.Set(other.Degree + 1, length, Mod<P>().Set(0));
			// NTT
			size_t loglength = Log2(length);
			Mod<P> zeta =
			Power(Mod<P>().Set((size_t)15311432), ((size_t)1 << (23 - loglength)));

			LightArray<Mod<P>> arr1 =
			fouriertransform_freq(arr, loglength, Mod<P>().Set(1) / zeta);

			LightArray<Mod<P>> arr2 =
			fouriertransform_freq(other.arr, loglength, Mod<P>().Set(1) / zeta);

			Array<Mod<P>> arr3(length);
			Mod<P> div = Mod<P>().Set(length).Inv();
			for (size_t i = 0; i < length; ++i) {
				arr3[i] = arr1[i] * arr2[i] * div;
			}

			FormalPowerSeries res;
			res.Degree = deg;
			res.arr = Array<Mod<P>>(fouriertransform_time(arr3, loglength, zeta));
			return res;
		}

		// Karatsuba
		// not implemented
		return FormalPowerSeries();
	}

	inline Mod<P>& operator[](size_t index) { return arr[index]; }

	FormalPowerSeries(FormalPowerSeries& src) = delete;
	FormalPowerSeries& operator=(FormalPowerSeries& src) = delete;

	FormalPowerSeries Copy() {
		FormalPowerSeries res(Degree);
		for (size_t i = 0; i <= Degree; ++i)
			res[i] = arr[i];
		return res;
	}

	FormalPowerSeries(FormalPowerSeries&& src) {
		arr = std::move(src.arr);
		Degree = src.Degree;
	}

	// move assignment operator
	FormalPowerSeries& operator=(FormalPowerSeries&& src) {
		arr = std::move(src.arr);
		Degree = src.Degree;
		return *this;
	}
};

template <size_t P> using FPS = FormalPowerSeries<Mod<P>>;

template <size_t P>
Mod<P> BostanMori(FPS<P>& p, FPS<P>& q, size_t N) { //[x^N] P(x)/Q(x),
	assert(q[0] != Mod<P>().Set(0));
	size_t degree = Min(N, Max(p.Degree, q.Degree) * 2);

	FPS<P> upper(degree);
	upper.Init();
	FPS<P> lower(degree);
	lower.Init();
	for (size_t i = 0; i <= N && i <= p.Degree && i <= degree; ++i)
		upper[i] = p[i];
	for (size_t i = 0; i <= N && i <= q.Degree && i <= degree; ++i)
		lower[i] = q[i];
	while (N != 0) {
		FPS<P> rev = lower.Copy();
		for (size_t i = 0; i <= degree; ++i) {
			if (i & 0x1) {
				rev[i] = Mod<P>().Set(0) - rev[i];
			}
		}
		upper = upper * rev;
		lower = lower * rev;
		size_t i;
		for (i = 0; 2 * i <= degree; ++i) {
			lower[i] = lower[2 * i];
		}
		size_t newlowerdeg = i - 1;
		size_t newupperdeg = 0;
		if (N % 2) {
			for (i = 0; 2 * i + 1 <= degree; ++i) {
				upper[i] = upper[2 * i + 1];
			}
			newupperdeg = i - 1;
		} else {
			for (i = 0; 2 * i <= degree; ++i) {
				upper[i] = upper[2 * i];
			}
			newupperdeg = i - 1;
		}
		N = N / 2;
		degree = Min(N, Max(newlowerdeg, newupperdeg) * 2);
		upper.arr.Set(newupperdeg + 1, degree + 1, Mod<P>().Set(0));
		lower.arr.Set(newlowerdeg + 1, degree + 1, Mod<P>().Set(0));
		upper.Degree = degree;
		lower.Degree = degree;
	}
	return (upper[0] / lower[0]);
}
