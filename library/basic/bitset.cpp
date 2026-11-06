#include "./base.cpp"

template <ull N> class BitSet {
	static constexpr ull arr_len = (N + 63) / 64;
	ull arr[arr_len];

public:
	bool operator[](ull ind) { return arr[ind >> 6] & ((ull)1 << (ind & 077)); }
	BitSet operator++() {
		for (ull i = 0; i < arr_len; ++i) {
			++arr[i];
			if (arr[i] != 0)
				break;
		}
		return *this;
	}
};
