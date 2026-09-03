#pragma once
#include "../standard.cpp"

template <typename T> class FormalPowerSeries;

// i<=Degree => arr[i] must be initialized by user

template <size_t P> class FormalPowerSeries<Mod<P>> {
public:
	LightArray<Mod<P>> arr;
	FormalPowerSeries(ull N) { SetDegree(N); }
	ull Degree;

	void Init() { arr.Set(0, Degree + 1, Mod<P>().Set(0)); }
	void Init(Mod<P> val) { arr.Set(0, Degree + 1, val); }

	void SetDegree(size_t deg) {
		arr.Allocate(deg + 1);
		Degree = deg;
		for (ull i = 0; i <= deg; ++i) {
			arr[i] = 0;
		}
	}

	FormalPowerSeries
	operator*(FormalPowerSeries& other) { // convolution of Degree(deg)
		return Prod(*this, other, Degree + other.Degree);
	}

	inline Mod<P>& operator[](size_t index) { return arr[index]; }

	FormalPowerSeries Copy() {
		FormalPowerSeries res(Degree);
		for (size_t i = 0; i <= Degree; ++i)
			res.arr[i] = arr[i];
		return res;
	}
};

template <size_t P> using FPS = FormalPowerSeries<Mod<P>>;

template <ull P> FPS<P> Prod(FPS<P>& f1, FPS<P>& f2, ull max_deg) {

	size_t deg = Min(f1.Degree + f2.Degree, max_deg);
	size_t length = BiggerPower2(deg * 2 + 1);
	size_t loglength = Log2(length);

	if (P == 998244353 && length <= (size_t)1 << 23) {
		// length = Max(length, (size_t)4); // degree>=2
		//  NTT

		Mod<P> zeta =
		  Power(Mod<P>().Set((size_t)15311432), ((size_t)1 << (23 - loglength)));

		LightArray<Mod<P>> arr_1, arr_2;
		fourier_transform_inverse(
		  f1.arr, f2.arr, arr_1, arr_2, length, zeta, Min(f1.Degree + 1, deg + 1),
		  Min(f2.Degree + 1, deg + 1)
		);

		LightArray<Mod<P>> arr_3(length);
		Mod<P> coef = Mod<P>(1) / Mod<P>(length);
		for (size_t i = 0; i < length; ++i) {
			arr_3[i] = arr_1[i] * arr_2[i] * coef;
		}

		FPS<P> res(deg);
		LightArray<Mod<P>> res_raw =
		  fourier_transform_forward(arr_3, length, zeta, length);
		for (ull i = 0; i <= deg; ++i)
			res.arr[i] = res_raw[i];
		return res;
	}
	return FPS<P>(0);
}

template <size_t P>
Mod<P> BostanMori(FPS<P>& p, FPS<P>& q, size_t N) { //[x^N] P(x)/Q(x),
	assert(q[0] != Mod<P>(0));
	FPS<P> upper = p.Copy();
	FPS<P> lower = q.Copy();

	while (N != 0) {
		FPS<P> lower_rev(lower.Degree);
		for (ull i = 0; i <= lower.Degree; ++i) {
			lower_rev.arr[i] = lower.arr[i];
			if (i & 0b1)
				lower_rev.arr[i] *= Mod<P>(P - 1);
		}
		upper = Prod(upper, lower_rev, N);
		lower = Prod(lower, lower_rev, N);

		FPS<P> new_lower(lower.Degree >> 1);
		for (ull i = 0; i <= new_lower.Degree; ++i) {
			new_lower.arr[i] = lower.arr[i << 1];
		}

		ull new_upper_degree = 0;
		if (upper.Degree > 0) {
			if (N & 0b1) {
				new_upper_degree = (upper.Degree - 1) / 2;
			} else {
				new_upper_degree = upper.Degree / 2;
			}
		}
		FPS<P> new_upper(new_upper_degree);
		for (ull i = 0; i <= new_upper.Degree; ++i) {
			new_upper.arr[i] = upper.arr[(i << 1) + (N & 0b1)];
		}
		N >>= 1;
		upper = std::move(new_upper);
		lower = std::move(new_lower);
	}
	return upper[0] / lower[0];
}

/* WIP
template <ull P> FPS<P> Prod(Array<FPS<P>>& arr) {

  AVLTree index(Type<ull>, Type<ull>); // check_next
  Array<ull> merge_cost;
  AVLTree avltree(Type<Tuple<ull, 2>>, Type<ull>);

  for (ull i = 0; i < arr.Length; ++i) {
    index.Push(i, )
  }
}
*/
