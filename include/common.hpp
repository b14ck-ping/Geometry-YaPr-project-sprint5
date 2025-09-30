#pragma once
#include <concepts>
#include <math.h>

const double tolerance__ = 1e-10;

template <std::floating_point T>
inline bool CheckDoubleIsZeroWithTolerance(T val) {
    return std::abs(val) < tolerance__;
}