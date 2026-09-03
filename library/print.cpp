#pragma once
#include <iomanip>
#include <iostream>

#include "basic.cpp"

template <typename D> void PrintDouble(D val) {
	cout << std::fixed << std::setprecision(16) << val;
}

template <typename T, typename... Args> void Print(T&& val, Args&&... args) {
	cout << std::forward<T>(val);
	((cout << ", " << std::forward<Args>(args)), ...);
	cout << '\n';
}

void PrintYes(bool flag) {
	if (flag)
		cout << "Yes" << '\n';
	else
		cout << "No" << '\n';
}

void PrintYES(bool flag) {
	if (flag)
		cout << "YES" << '\n';
	else
		cout << "NO" << '\n';
}

void Printyes(bool flag) {
	if (flag)
		cout << "yes" << '\n';
	else
		cout << "no" << '\n';
}
