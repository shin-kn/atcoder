#pragma once
#include "base.cpp"
#include "math.cpp"

template <typename T> class LightArray {
public:
	T* arr = nullptr;
	size_t Length = 0;

	LightArray() {}
	LightArray(size_t n) { Allocate(n); }
	LightArray(T* ptr, size_t n) : arr(ptr), Length(n) {}
	LightArray(size_t n, T val) {
		Allocate(n);
		Set(val);
	}

	inline void Set(T val) { this->Set(0, Length, val); }
	inline void Set(size_t start, size_t end, T val);

	void Allocate(size_t n);

	inline T& operator[](size_t idx) { return arr[idx]; }
	inline T& operator()(ull idx) { return arr[idx]; }

	LightArray(LightArray& src) = delete;
	LightArray(LightArray&& src);
	LightArray& operator=(LightArray& src) = delete;

	LightArray& operator=(LightArray&& other);

	~LightArray() {
		if (arr != nullptr) {
			delete[] arr;
		}
	}
};

template <typename T> void LightArray<T>::Set(size_t start, size_t end, T val) {
	assert(std::is_copy_assignable<T>::value);
	for (size_t i = start; i < end; ++i) {
		if constexpr (std::is_copy_assignable<T>::value) {
			(*this)[i] = val;
		}
	}
}

template <typename T> void LightArray<T>::Allocate(size_t n) {
	Length = n;
	arr = new (std::nothrow) T[n];
	if (arr == nullptr)
		std::exit(EXIT_FAILURE);
}

template <typename T> LightArray<T>::LightArray(LightArray&& src) {
	arr = src.arr;
	Length = src.Length;
	src.arr = nullptr;
	src.Length = 0;
}
template <typename T>
LightArray<T>& LightArray<T>::operator=(LightArray&& other) {
	if (this == &other)
		return *this;
	if (arr != nullptr)
		delete[] arr;
	arr = other.arr;
	Length = other.Length;
	other.arr = nullptr;
	other.Length = 0;
	return *this;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, LightArray<T>& arr) {
	os << "[";
	for (ull i = 0; i < arr.Length; ++i) {
		if (i != 0)
			cout << ", ";
		cout << arr[i];
	}
	cout << "]";
	return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, LightArray<T>&& arr) {
	cout << arr;
	return os;
}
