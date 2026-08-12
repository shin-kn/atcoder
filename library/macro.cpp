#pragma once

#define rep(i, n) for (size_t i = 0; i < (size_t)(n); ++i)
#define rep_3(i, m, n) for (size_t i = (m); i < (size_t)(n); ++i)
#define rep_minus(i, n)                                                        \
	for (size_t i = (size_t)(n) - (size_t)1; i != (size_t)-1; --i)

#define range(i, v, arr)                                                       \
	if (arr.Length > 0)                                                          \
		for (size_t i = 0, RANGE_LOOP_TEMP = 0; i < arr.Length;                    \
		     (RANGE_LOOP_TEMP = 0, ++i))                                           \
			for ([[maybe_unused]] auto&& v = arr[i]; RANGE_LOOP_TEMP == 0;           \
			     ++RANGE_LOOP_TEMP)
