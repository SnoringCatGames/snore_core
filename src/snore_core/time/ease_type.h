#ifndef EASE_TYPE_H
#define EASE_TYPE_H

#include "snore_core/internal/string_utils.h"

#include <godot_cpp/core/class_db.hpp>

namespace godot {

enum EaseType {
	LINEAR,
	EASE_IN,
	EASE_IN_STRONG,
	EASE_IN_VERY_STRONG,
	EASE_IN_WEAK,
	EASE_OUT,
	EASE_OUT_STRONG,
	EASE_OUT_VERY_STRONG,
	EASE_OUT_WEAK,
	EASE_IN_OUT,
	EASE_IN_OUT_STRONG,
	EASE_IN_OUT_VERY_STRONG,
	EASE_IN_OUT_WEAK,

	_EaseType_COUNT,
};

static constexpr const char *ease_type_strings[EaseType::_EaseType_COUNT] = {
	"LINEAR",
	"EASE_IN",
	"EASE_IN_STRONG",
	"EASE_IN_VERY_STRONG",
	"EASE_IN_WEAK",
	"EASE_OUT",
	"EASE_OUT_STRONG",
	"EASE_OUT_VERY_STRONG",
	"EASE_OUT_WEAK",
	"EASE_IN_OUT",
	"EASE_IN_OUT_STRONG",
	"EASE_IN_OUT_VERY_STRONG",
	"EASE_IN_OUT_WEAK",
};

static String ease_type_to_string(EaseType p_ease_type) {
	return ease_type_strings[p_ease_type];
}

static String get_ease_type_hint_string() {
	return join_strings(ease_type_strings, EaseType::_EaseType_COUNT, ",");
}

} // namespace godot

VARIANT_ENUM_CAST(EaseType);

#endif // EASE_TYPE_H
