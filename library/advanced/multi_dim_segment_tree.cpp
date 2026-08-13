#include "../standard.cpp"

template <typename T, ull N> class SegmentTree {
	static_assert(std::is_copy_assignable<T>::value);

public:
	template <typename AddFunc> SegmentTree(ull N, T val, AddFunc&& func) {
		Init(N, val, std::forward<AddFunc>(func));
	}
};
