#pragma once
#include "base.cpp"
#include "function_type.cpp"
#include "min_max.cpp"

template <typename T>
FunctionType<T(T,T)> AddFunc(){
    static_assert(std::is_copy_assignable<T>::value);
    static auto addfunc=[](T val1,T val2)->T{
        return val1+val2;
    };
    return FunctionType<T(T,T)>(addfunc);
}


template <typename T>
FunctionType<T(T,T)> MinFunc(){
    static_assert(std::is_copy_assignable<T>::value);
    static auto minfunc=[](T val1,T val2)->T{
        return Min(val1,val2);
    };
    return FunctionType<T(T,T)>(minfunc);
}

template <typename T>
FunctionType<T(T,T)> MaxFunc(){
    static_assert(std::is_copy_assignable<T>::value);
    static auto maxfunc=[](T val1,T val2)->T{
        return Max(val1,val2);
    };
    return FunctionType<T(T,T)>(maxfunc);
}


template <typename T>
FunctionType<bool(T,T)> SmallerFunc()
requires SmallerDefined<T>
{
    static_assert(std::is_copy_assignable<T>::value);
    static auto smallerfunc=[](T val1,T val2)->bool{
        return val1<val2;
    };
    return FunctionType<bool(T,T)>(smallerfunc);
}


template <typename T>
FunctionType<bool(T,T)> BiggerFunc()
requires BiggerDefined<T>
{
    static_assert(std::is_copy_assignable<T>::value);
    static auto biggerfunc=[](T val1,T val2)->bool{
        return val1>val2;
    };
    return FunctionType<bool(T,T)>(biggerfunc);
}

