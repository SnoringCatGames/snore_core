#ifndef ELAPSED_TIME_TYPE_HE
#define ELAPSED_TIME_TYPE_HE

#include "snore_core/internal/debug_utils.h"
#include "snore_core/internal/string_utils.h"
#include "snore_core/time/time_type.h"

#include <godot_cpp/core/class_db.hpp>

namespace godot {

enum ElapsedTimeType {
	ELAPSED_PHYSICS_TIME,
	ELAPSED_PHYSICS_SCALED_TIME,
	ELAPSED_CLOCK_TIME,
	ELAPSED_CLOCK_SCALED_TIME,
	ELAPSED_RENDER_TIME,
	ELAPSED_RENDER_SCALED_TIME,
	PHYSICS_FRAME_COUNT,
	RENDER_FRAME_COUNT,

	_ElapsedTimeType_COUNT,
};

static constexpr const char
		*elapsed_time_type_strings[ElapsedTimeType::_ElapsedTimeType_COUNT] = {
			"ELAPSED_PHYSICS_TIME", "ELAPSED_PHYSICS_SCALED_TIME",
			"ELAPSED_CLOCK_TIME",	"ELAPSED_CLOCK_SCALED_TIME",
			"ELAPSED_RENDER_TIME",	"ELAPSED_RENDER_SCALED_TIME",
			"PHYSICS_FRAME_COUNT",	"RENDER_FRAME_COUNT",
		};

static String elapsed_time_type_to_string(ElapsedTimeType p_elapsed_time_type) {
	return elapsed_time_type_strings[p_elapsed_time_type];
}

static String get_elapsed_time_type_hint_string() {
	return join_strings(
			elapsed_time_type_strings, ElapsedTimeType::_ElapsedTimeType_COUNT,
			",");
}

ElapsedTimeType get_elapsed_time_type_for_time_type(TimeType p_time_type);

TimeType get_time_type_from_elapsed_time_type(ElapsedTimeType p_elapsed_type);

} // namespace godot

VARIANT_ENUM_CAST(ElapsedTimeType);

#endif // ELAPSED_TIME_TYPE_HE
