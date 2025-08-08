#include "snore_core/rotated_shape.h"

#include "snore_core/geometry.h"
#include "snore_core/internal/debug_utils.h"
#include "snore_core/internal/ref_utils.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

bool RotatedShape::get_is_rotated_90_degrees() const {
	return !Math::is_inf(rotation) &&
			Math::abs(fmod(rotation + tau, pi) - half_pi) < float_epsilon;
}

bool RotatedShape::get_is_axially_aligned() const {
	if (Math::is_inf(rotation)) {
		return false;
	}
	const float remainder = fmod(rotation + tau, pi);
	return Math::abs(remainder) < float_epsilon ||
			Math::abs(remainder - half_pi) < float_epsilon;
}

void RotatedShape::set_up(const Ref<Shape2D> &p_shape, double p_rotation) {
	if (is_valid(p_shape)) {
		shape = p_shape;
	}
	if (!Math::is_inf(p_rotation)) {
		rotation = p_rotation;
	}

	if (!is_valid(shape) || Math::is_inf(rotation)) {
		return;
	}

	if (!get_is_axially_aligned()) {
		// TODO: Add suppport (and tests) for non-axially-aligned shapes.
		ENSURE(false,
			   "RotatedShape::update: Non-axially-aligned shapes are not "
			   "currently supported.");
		half_width_height = vector2_invalid;
		return;
	}

	half_width_height = Geometry::calculate_half_width_height(
			shape, get_is_rotated_90_degrees());
}

void RotatedShape::reset() {
	shape.unref();
	rotation = inf;
	half_width_height = vector2_invalid;
}

void RotatedShape::set_shape(const Ref<Shape2D> &p_shape) { set_up(p_shape); }

void RotatedShape::set_rotation(double p_rotation) {
	set_up(nullptr, p_rotation);
}

void RotatedShape::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_shape"), &RotatedShape::get_shape);
	ClassDB::bind_method(
			D_METHOD("set_shape", "shape"), &RotatedShape::set_shape);
	ADD_PROPERTY(
			PropertyInfo(
					Variant::OBJECT, "shape", PROPERTY_HINT_RESOURCE_TYPE,
					"Shape2D"),
			"set_shape", "get_shape");

	ClassDB::bind_method(D_METHOD("get_rotation"), &RotatedShape::get_rotation);
	ClassDB::bind_method(
			D_METHOD("set_rotation", "rotation"), &RotatedShape::set_rotation);
	ADD_PROPERTY(
			PropertyInfo(Variant::FLOAT, "rotation"), "set_rotation",
			"get_rotation");

	ClassDB::bind_method(
			D_METHOD("get_is_rotated_90_degrees"),
			&RotatedShape::get_is_rotated_90_degrees);
	ADD_PROPERTY(
			PropertyInfo(Variant::BOOL, "is_rotated_90_degrees"), "",
			"get_is_rotated_90_degrees");

	ClassDB::bind_method(
			D_METHOD("get_is_axially_aligned"),
			&RotatedShape::get_is_axially_aligned);
	ADD_PROPERTY(
			PropertyInfo(Variant::BOOL, "is_axially_aligned"), "",
			"get_is_axially_aligned");

	ClassDB::bind_method(
			D_METHOD("get_half_width_height"),
			&RotatedShape::get_half_width_height);
	ADD_PROPERTY(
			PropertyInfo(Variant::VECTOR2, "half_width_height"), "",
			"get_half_width_height");

	ClassDB::bind_method(
			D_METHOD("set_up", "shape", "rotation"), &RotatedShape::set_up,
			DEFVAL(Ref<Shape2D>()), DEFVAL(inf));
	ClassDB::bind_method(D_METHOD("reset"), &RotatedShape::reset);
}
