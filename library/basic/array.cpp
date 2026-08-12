#pragma once

#include "base.cpp"
#include "light_array.cpp"
#include "math.cpp"

constexpr ull SMALL_TYPE = 100;
constexpr ull SMALL_LENGTH = 8;

template <typename T, bool Unsafe = false> class Array {
	T** arr = nullptr;

	T** pointers = nullptr;
	size_t pointerscapacity = 0;
	size_t pointerslength = 0;

public:
	size_t Length = 0;
	size_t Capacity = 0;

	Array() {}
	Array(size_t n) {
		Allocate(n);
		Length = n;
	}
	Array(size_t n, T val) {
		Allocate(n);
		Length = n;
		Set(val);
	}

	Array(LightArray<T>&& other);

	T& Last() { return (*this)[Length - 1]; }

	~Array();

	inline void Set(size_t start, size_t end, T val);

	inline void Set(T val) { this->Set(0, Length, val); }

	inline T& operator[](size_t idx);
	inline T& operator()(size_t idx);

	inline void Push(T val);

	inline void Allocate(ull cap);

	Array(Array& src) = delete;

	Array(Array&& src);

	// copy assignment operator
	Array& operator=(Array& src) = delete;

	// move assignment operator
	Array& operator=(Array&& src);

	inline void Clear();
};

template <typename T, bool Unsafe> void Array<T, Unsafe>::Push(T val) {
	if constexpr (Unsafe) {
		(*this)(Length) = val;
	} else {
		(*this)[Length] = val;
	}
}

template <typename T, bool Unsafe> void Array<T, Unsafe>::Clear() {
	arr = nullptr;
	Length = 0;
	Capacity = 0;
	pointers = nullptr;
	pointerscapacity = 0;
	pointerslength = 0;
}

template <typename T, bool Unsafe> Array<T, Unsafe>::~Array() {
	if (arr != nullptr) {
		delete[] arr;
	}
	for (size_t i = 0; i < pointerslength; ++i) {
		delete[] pointers[i];
	}
	if (pointers != nullptr)
		delete[] pointers;
}

template <typename T, bool Unsafe>
Array<T, Unsafe>::Array(LightArray<T>&& other) {
	Length = other.Length;
	Capacity = other.Length;
	pointerscapacity = 1;
	pointerslength = 1;
	pointers = new (std::nothrow) T*[pointerscapacity];
	if (pointers == nullptr)
		std::exit(EXIT_FAILURE);
	pointers[0] = other.arr;

	arr = new (std::nothrow) T*[Length];
	if (arr == nullptr)
		std::exit(EXIT_FAILURE);
	for (size_t i = 0; i < Length; ++i) {
		arr[i] = &(other.arr[i]);
	}
	other.arr = nullptr;
	other.Length = 0;
}

template <typename T, bool Unsafe>
void Array<T, Unsafe>::Set(ull start, ull end, T val) {
	for (size_t i = start; i < end; ++i) {
		if constexpr (std::is_copy_assignable<T>::value) {
			(*this)[i] = val;
		}
	}
}

template <typename T, bool Unsafe>
inline T& Array<T, Unsafe>::operator[](ull idx) {
	if constexpr (!Unsafe) {
		if (Length <= idx) {
			if (Capacity <= idx) {
				Allocate(idx + 1);
			}
			Length = idx + 1;
		}
	}
	return *(arr[idx]);
}

template <typename T, bool Unsafe>
inline T& Array<T, Unsafe>::operator()(ull idx) {
	if constexpr (Unsafe) {
		if (Length <= idx) {
			if (Capacity <= idx) {
				Allocate(idx + 1);
			}
			Length = idx + 1;
		}
	}
	return *(arr[idx]);
}
template <typename T, bool Unsafe>
inline void Array<T, Unsafe>::Allocate(ull cap) {
	if (cap <= Capacity)
		return;
	cap = BiggerPower2(cap);
	if constexpr (sizeof(T) <= SMALL_TYPE) {
		if (Capacity == 0) {
			if (cap < SMALL_LENGTH) {
				cap = BiggerPower2(SMALL_LENGTH);
			}
		}
	}

	T** oldarr = arr;
	arr = new (std::nothrow) T*[cap];
	if (arr == nullptr)
		std::exit(EXIT_FAILURE);
	for (size_t i = 0; i < Capacity; ++i) {
		arr[i] = oldarr[i];
	}
	if (oldarr != nullptr)
		delete[] oldarr;

	if (pointerslength == pointerscapacity) {
		pointerscapacity = BiggerPower2(pointerscapacity + 1);
		T** oldpointers = pointers;
		pointers = new (std::nothrow) T*[pointerscapacity];
		if (pointers == nullptr)
			std::exit(EXIT_FAILURE);
		for (size_t i = 0; i < pointerslength; ++i) {
			pointers[i] = oldpointers[i];
		}
		if (oldpointers != nullptr)
			delete[] oldpointers;
	}
	T* newpointer = new (std::nothrow) T[cap - Capacity];
	if (newpointer == nullptr)
		std::exit(EXIT_FAILURE);
	pointers[pointerslength] = newpointer;
	++pointerslength;
	for (size_t i = 0; i < cap - Capacity; ++i) {
		arr[Capacity + i] = &(newpointer[i]);
	}
	Capacity = cap;
}

template <typename T, bool Unsafe>
Array<T, Unsafe>::Array(Array<T, Unsafe>&& src) {
	arr = src.arr;
	Length = src.Length;
	Capacity = src.Capacity;
	pointers = src.pointers;
	pointerslength = src.pointerslength;
	pointerscapacity = src.pointerscapacity;

	src.Clear();
}

template <typename T, bool Unsafe>
Array<T, Unsafe>& Array<T, Unsafe>::operator=(Array<T, Unsafe>&& src) {
	if (&src == this) {
		return *this;
	}
	// destructor
	if (arr != nullptr) {
		delete[] arr;
	}
	for (size_t i = 0; i < pointerslength; ++i) {
		delete[] pointers[i];
	}
	if (pointers != nullptr)
		delete[] pointers;
	arr = src.arr;
	Length = src.Length;
	Capacity = src.Capacity;
	pointers = src.pointers;
	pointerslength = src.pointerslength;
	pointerscapacity = src.pointerscapacity;
	src.Clear();
	return *this;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, Array<T>& arr) {
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
std::ostream& operator<<(std::ostream& os, Array<T>&& arr) {
	cout << arr;
	return os;
}
