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
	bool operator>(Tuple<T, N> other) {
		for (size_t i = 0; i < N; ++i) {
			if (this->Val[i] > other.Val[i]) {
				return true;
			}
			if (this->Val[i] < other.Val[i]) {
				return false;
			}
		}
		return false;
	}

	bool operator<=(Tuple<T, N> other) { return !(this->operator>(other)); }
	bool operator>=(Tuple<T, N> other) { return !(this->operator<(other)); }

	bool operator==(Tuple<T, N>& other) {
		for (size_t i = 0; i < N; ++i) {
			if (this->Val[i] != other.Val[i]) {
				return false;
			}
		}
		return true;
	}
};

template <typename T, typename... Args>
Tuple(T, Args...) -> Tuple<T, sizeof...(Args) + 1>;

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

	bool operator<(Pair other)
	  requires SmallerDefined<T1> && SmallerDefined<T2>;
	Pair operator+(Pair other)
	  requires requires(Pair a, Pair b) {
		  a.val1 + b.val1;
		  a.val2 + b.val2;
	  }
	{
		return Pair(val1 + other.val1, val2 + other.val2);
	}
};

template <typename T1, typename T2>
bool Pair<T1, T2>::operator<(Pair<T1, T2> other)
  requires SmallerDefined<T1> && SmallerDefined<T2>
{
	if (val1 < other.val1 || other.val1 < val1) {
		return val1 < other.val1;
	}
	return val2 < other.val2;
}

template <typename T1, typename T2>
std::ostream& operator<<(std::ostream& os, Pair<T1, T2> val) {
	os << "(" << val.val1 << "," << val.val2 << ")";
	return os;
}
