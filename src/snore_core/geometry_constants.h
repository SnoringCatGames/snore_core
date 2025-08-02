#ifndef GEOMETRY_CONSTANTS_H
#define GEOMETRY_CONSTANTS_H

#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace godot {

#define infinity INFINITY
constexpr float float_epsilon = 0.00001f;
constexpr double tau = Math_TAU;
constexpr double pi = Math_PI;
constexpr double half_pi = pi / 2.0;
constexpr double quarter_pi = pi / 4.0;

// Infinity is used rather than NaN to avoid issues with NaN comparisons.
static const Vector2 vector2_invalid = Vector2(infinity, infinity);
static const Vector2 vector2_zero = Vector2(0, 0);
static const Vector2 vector2_one = Vector2(1, 1);
static const Vector2 vector2_up = Vector2(0, -1);
static const Vector2 vector2_down = Vector2(0, 1);
static const Vector2 vector2_left = Vector2(-1, 0);
static const Vector2 vector2_right = Vector2(1, 0);

static const Vector3 vector3_invalid = Vector3(infinity, infinity, infinity);
static const Vector3 vector3_zero = Vector3(0, 0, 0);

} //namespace godot

#endif // GEOMETRY_CONSTANTS_H
