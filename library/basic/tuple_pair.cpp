#pragma once
#include "base.cpp"
template <typename T, typename... Args> using FirstArgType = T;

template <typename T, size_t N> class Tuple {
	T Val[N];

public:
	constexpr static ull Length = N;

	Tuple() = default;
	template <typename... Args>
	  requires(sizeof...(Args) == N)
	constexpr Tuple(Args... args) : Val(args...) {}

	constexpr T& operator[](const size_t idx) { return Val[idx]; }
	constexpr const T& operator[](const size_t idx) const { return Val[idx]; }

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
	  requires requires(T a, T b) { a - b; }
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

	auto operator<=>(const Tuple&) const = default;
};

template <typename T, typename... Args>
Tuple(T, Args...) -> Tuple<std::remove_cvref_t<T>, sizeof...(Args) + 1>;

template <typename T, ull N> std::ostream& operator<<(std::ostream& os, const Tuple<T, N>& tuple) {
	os << "(";
	for (ull i = 0; i < N; ++i) {
		if (i != 0)
			os << ", ";
		os << tuple[i];
	}
	os << ")";
	return os;
}

template <typename T, ull N> std::istream& operator>>(std::istream& is, Tuple<T, N>& tuple) {
	for (ull i = 0; i < N; ++i)
		is >> tuple[i];
	return is;
}

template <typename T1, typename T2> class Pair {

public:
	template <typename S1, typename S2>
	Pair(S1&& v1, S2&& v2) : v1(std::forward<S1>(v1)), v2(std::forward<S2>(v2)) {}
	Pair() : v1(T1()), v2(T2()) {}

	T1 v1;
	T2 v2;

	Pair operator+(Pair other)
	  requires requires(Pair a, Pair b) {
		  a.v1 + b.v1;
		  a.v2 + b.v2;
	  }
	{
		return Pair(v1 + other.v1, v2 + other.v2);
	}
	auto operator<=>(const Pair&) const = default;
};

template <typename T, typename U>
Pair(T, U) -> Pair<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

template <typename T1, typename T2>
std::ostream& operator<<(std::ostream& os, const Pair<T1, T2>& val) {
	os << "(" << val.v1 << "," << val.v2 << ")";
	return os;
}

template <typename T1, typename T2> std::istream& operator>>(std::istream& is, Pair<T1, T2>& val) {
	is >> val.v1 >> val.v2;
	return is;
}

template <typename T1, typename T2, typename T3> class Trio {

public:
	template <typename S1, typename S2, typename S3>
	Trio(S1&& v1, S2&& v2, S3&& v3)
	    : v1(std::forward<S1>(v1)), v2(std::forward<S2>(v2)), v3(std::forward<S3>(v3)) {}
	Trio() : v1(T1()), v2(T2()), v3(T3()) {}

	T1 v1;
	T2 v2;
	T3 v3;

	Trio operator+(Trio other)
	  requires requires(Trio a, Trio b) {
		  a.v1 + b.v1;
		  a.v2 + b.v2;
		  a.v3 + b.v3;
	  }
	{
		return Trio(v1 + other.v1, v2 + other.v2, v3 + other.v3);
	}
	auto operator<=>(const Trio&) const = default;
};

template <typename T, typename U, typename V>
Trio(T, U, V) -> Trio<std::remove_cvref_t<T>, std::remove_cvref_t<U>, std::remove_cvref_t<V>>;

template <typename T1, typename T2, typename T3>
std::ostream& operator<<(std::ostream& os, const Trio<T1, T2, T3>& val) {
	os << "(" << val.v1 << "," << val.v2 << "," << val.v3 << ")";
	return os;
}

template <typename T1, typename T2, typename T3>
std::istream& operator>>(std::istream& is, Trio<T1, T2, T3>& val) {
	is >> val.v1 >> val.v2 >> val.v3;
	return is;
}
