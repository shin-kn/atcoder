#pragma once
#include "../standard.cpp"

template <typename T> class FormalPowerSeries;

// i<=Degree => arr[i] must be initialized by user

template <> class FormalPowerSeries<NMod> {
public:
	LightArray<NMod> arr;
	ull Degree = 0;
	FormalPowerSeries() {};
	FormalPowerSeries(ull N) { Allocate(N); };

	void Allocate(ull N);

	FormalPowerSeries
	operator*(FormalPowerSeries& other) { // convolution of Degree(deg)
		ull deg = Degree + other.Degree;
		ull length = BiggerPower2(deg * 2 + 1);
		length = Max(length, (size_t)4); // degree>=2
		assert(length <= (size_t)1 << 23);

		// NTT
		ull loglength = Log2(length);

		arr.Set(Degree + 1, length, Mod<P>().Set(0));
		other.arr.Set(other.Degree + 1, length, Mod<P>().Set(0));
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

	inline Mod<P>& operator[](size_t index) { return arr[index]; }
};

void FormalPowerSeries<NMod>::Allocate(ull N) {
	LightArray<NMod> newarr;
	newarr.Allocate(N + 1);
	for (ull i = 0; i < Min(N + 1, arr.Length); ++i) {
		newarr[i] = arr[i];
	}
	for (ull i = Min(N + 1, arr.Length); i < N + 1; ++i) {
		arr[i] = 0;
	}
	Degree = N;
}

template <size_t P> using FPS = FormalPowerSeries<Mod<P>>;
/*
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
}*/

template <ull P> FPS<P> Prod(Array<FPS<P>>& arr) {

	AVLTree index(Type<ull>, Type<ull>); // check_next
	Array<ull> merge_cost;
	AVLTree avltree(Type<Set<ull, 2>>, Type<ull>);

	for (ull i = 0; i < arr.Length; ++i) {
		index.Push(i, )
	}
}
