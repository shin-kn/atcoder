#pragma once
#include <iomanip>
#include <iostream>

#include "basic.cpp"

inline std::ostream& PrintEndl(std::ostream& os) {
	if constexpr (PrintWithEndl) {
		return os << endl;
	} else {
		return os << '\n';
	}
}

template <typename D> void PrintDouble(D val) {
	cout << std::fixed << std::setprecision(16) << val;
}

template <typename T, typename... Args> void Print(T&& val, Args&&... args) {
	cout << std::forward<T>(val);
	((cout << ", " << std::forward<Args>(args)), ...);
	cout << PrintEndl;
}

void PrintYes(bool flag) {
	if (flag)
		cout << "Yes" << PrintEndl;
	else
		cout << "No" << PrintEndl;
}

void PrintYES(bool flag) {
	if (flag)
		cout << "YES" << PrintEndl;
	else
		cout << "NO" << PrintEndl;
}

void Printyes(bool flag) {
	if (flag)
		cout << "yes" << PrintEndl;
	else
		cout << "no" << PrintEndl;
}
