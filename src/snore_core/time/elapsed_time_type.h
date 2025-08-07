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

ElapsedTimeType get_elapsed_time_type_for_time_type(TimeType p_time_type) {
	switch (p_time_type) {
		case TimeType::APP_PHYSICS:
		case TimeType::PLAY_PHYSICS:
			return ElapsedTimeType::ELAPSED_PHYSICS_TIME;
		case TimeType::APP_PHYSICS_SCALED:
		case TimeType::PLAY_PHYSICS_SCALED:
			return ElapsedTimeType::ELAPSED_PHYSICS_SCALED_TIME;
		case TimeType::APP_CLOCK:
			return ElapsedTimeType::ELAPSED_CLOCK_TIME;
		case TimeType::APP_CLOCK_SCALED:
			return ElapsedTimeType::ELAPSED_CLOCK_SCALED_TIME;
		case TimeType::PLAY_RENDER:
			return ElapsedTimeType::ELAPSED_RENDER_TIME;
		case TimeType::PLAY_RENDER_SCALED:
			return ElapsedTimeType::ELAPSED_RENDER_SCALED_TIME;
		case TimeType::APP_PHYSICS_FRAME_COUNT:
		case TimeType::PLAY_PHYSICS_FRAME_COUNT:
			return ElapsedTimeType::PHYSICS_FRAME_COUNT;
		case TimeType::APP_RENDER_FRAME_COUNT:
		case TimeType::PLAY_RENDER_FRAME_COUNT:
			return ElapsedTimeType::RENDER_FRAME_COUNT;
		default:
			ENSURE(false, vformat("Unrecognized time_type: %d", p_time_type));
			return ElapsedTimeType::ELAPSED_PHYSICS_TIME;
	}
}

TimeType get_time_type_from_elapsed_time_type(ElapsedTimeType p_elapsed_type) {
	switch (p_elapsed_type) {
		case ElapsedTimeType::ELAPSED_PHYSICS_TIME:
			return TimeType::APP_PHYSICS;
		case ElapsedTimeType::ELAPSED_PHYSICS_SCALED_TIME:
			return TimeType::APP_PHYSICS_SCALED;
		case ElapsedTimeType::ELAPSED_CLOCK_TIME:
			return TimeType::APP_CLOCK;
		case ElapsedTimeType::ELAPSED_CLOCK_SCALED_TIME:
			return TimeType::APP_CLOCK_SCALED;
		case ElapsedTimeType::ELAPSED_RENDER_TIME:
			return TimeType::PLAY_RENDER;
		case ElapsedTimeType::ELAPSED_RENDER_SCALED_TIME:
			return TimeType::PLAY_RENDER_SCALED;
		case ElapsedTimeType::PHYSICS_FRAME_COUNT:
			return TimeType::APP_PHYSICS_FRAME_COUNT;
		case ElapsedTimeType::RENDER_FRAME_COUNT:
			return TimeType::APP_RENDER_FRAME_COUNT;
		default:
			ENSURE(false,
				   vformat("Unrecognized elapsed_time_type: %d",
						   p_elapsed_type));
			return TimeType::APP_PHYSICS;
	}
}

} // namespace godot

VARIANT_ENUM_CAST(ElapsedTimeType);

#endif // ELAPSED_TIME_TYPE_HE
