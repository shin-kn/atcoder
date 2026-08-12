#pragma once
#include "base.cpp"
#include "function_type.cpp"
#include "min_max.cpp"

template <typename T>
auto AddOp = [](T val1, T val2) -> T { return val1 + val2; };

template <typename T>
auto MinOp = [](T val1, T val2) -> T { return Min(val1, val2); };

template <typename T>
auto MaxOp = [](T val1, T val2) -> T { return Max(val1, val2); };

template <typename T>
auto LTOp = [](T val1, T val2) -> T { return val1 < val2; };

template <typename T>
auto GTOp = [](T val1, T val2) -> T { return val1 > val2; };

template <typename TResult, typename... TArgs>
auto DoNothingFunc = [](TArgs... args) { return TResult(); };
