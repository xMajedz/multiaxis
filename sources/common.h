#pragma once
#include "ode/ode.h"

#include "raylib/raylib.hpp"

#include <cstdint>

#include <iostream>

#include <map>
#include <unordered_map>
#include <array>
#include <vector>
#include <string>
#include <string_view>
#include <sstream>

template<typename T>
struct vec3
{
	T x;
	T y;
	T z;
};

template<typename T>
struct vec4
{
	T x;
	T y;
    T z;
	T w;
};
