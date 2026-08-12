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
	cout << endl;
}

void PrintYes(bool flag) {
	if (flag)
		cout << "Yes" << endl;
	else
		cout << "No" << endl;
}

void PrintYES(bool flag) {
	if (flag)
		cout << "YES" << endl;
	else
		cout << "NO" << endl;
}

void Printyes(bool flag) {
	if (flag)
		cout << "yes" << endl;
	else
		cout << "no" << endl;
}
