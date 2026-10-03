#pragma once
#include "base.cpp"
template <typename T, typename... Args> using FirstArgType = T;

template <typename T, size_t N> class Tuple {
	T Val[N];

public:
	ull Length = N;

	Tuple() {}

	template <typename... Args>
	  requires(sizeof...(Args) == N)
	Tuple(Args... args) : Val(args...) {}

	Tuple& operator=(const Tuple& src) {
		for (size_t i = 0; i < N; ++i) {
			Val[i] = src.Val[i];
		}
		return *this;
	}

	T& operator[](size_t idx) { return Val[idx]; }

	Tuple operator+(const Tuple& other)
	  requires requires(T a, T b) { a + b; }
	{
		Tuple res;
		for (size_t i = 0; i < N; ++i) {
			res.Val[i] = Val[i] + other.Val[i];
		}
		return res;
	}
	Tuple operator-(const Tuple& other)
	  requires requires(T a, T b) { a + b; }
	{
		Tuple res;
		for (size_t i = 0; i < N; ++i) {
			res.Val[i] = Val[i] - other.Val[i];
		}
		return res;
	}
	Tuple& operator+=(const Tuple& other)
	  requires requires(T a, T b) { a + b; }
	{
		for (ull i = 0; i < N; ++i) {
			Val[i] += other.Val[i];
		}
		return *this;
	}

	bool operator<(Tuple<T, N> other) {
		for (size_t i = 0; i < N; ++i) {
			if (this->Val[i] < other.Val[i]) {
				return true;
			}
			if (this->Val[i] > other.Val[i]) {
				return false;
			}
		}
		return false;
	}

	auto operator<=>(const Tuple&) const = default;
};

template <typename T, typename... Args>
Tuple(T, Args...) -> Tuple<std::remove_cvref_t<T>, sizeof...(Args) + 1>;

template <typename T, ull N>
std::ostream& operator<<(std::ostream& os, Tuple<T, N>& tuple) {
	os << "(";
	for (ull i = 0; i < N; ++i) {
		if (i != 0)
			cout << ", ";
		cout << tuple[i];
	}
	cout << ")";
	return os;
}

template <typename T1, typename T2> class Pair {
	static_assert(std::is_copy_assignable<T1>::value);
	static_assert(std::is_copy_assignable<T2>::value);

public:
	Pair(T1 val1, T2 val2) : val1(val1), val2(val2) {}
	Pair() : val1(T1()), val2(T2()) {}

	T1 val1;
	T2 val2;

	Pair operator+(Pair other)
	  requires requires(Pair a, Pair b) {
		  a.val1 + b.val1;
		  a.val2 + b.val2;
	  }
	{
		return Pair(val1 + other.val1, val2 + other.val2);
	}
	auto operator<=>(const Pair&) const = default;
};

template <typename T, typename U>
Pair(T, U) -> Pair<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

template <typename T1, typename T2>
std::ostream& operator<<(std::ostream& os, Pair<T1, T2> val) {
	os << "(" << val.val1 << "," << val.val2 << ")";
	return os;
}
