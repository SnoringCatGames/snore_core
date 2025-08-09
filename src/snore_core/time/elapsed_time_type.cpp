#include "elapsed_time_type.h"

namespace godot {

ElapsedTimeType get_elapsed_time_type_for_time_type(TimeType p_time_type) {
	switch (p_time_type) {
		case APP_PHYSICS:
		case PLAY_PHYSICS:
			return ELAPSED_PHYSICS_TIME;
		case APP_PHYSICS_SCALED:
		case PLAY_PHYSICS_SCALED:
			return ELAPSED_PHYSICS_SCALED_TIME;
		case APP_CLOCK:
			return ELAPSED_CLOCK_TIME;
		case APP_CLOCK_SCALED:
			return ELAPSED_CLOCK_SCALED_TIME;
		case PLAY_RENDER:
			return ELAPSED_RENDER_TIME;
		case PLAY_RENDER_SCALED:
			return ELAPSED_RENDER_SCALED_TIME;
		case APP_PHYSICS_FRAME_COUNT:
		case PLAY_PHYSICS_FRAME_COUNT:
			return PHYSICS_FRAME_COUNT;
		case APP_RENDER_FRAME_COUNT:
		case PLAY_RENDER_FRAME_COUNT:
			return RENDER_FRAME_COUNT;
		default:
			ENSURE(false, "Invalid TimeType: " + itos((int)p_time_type));
			return ELAPSED_PHYSICS_TIME;
	}
}

TimeType get_time_type_from_elapsed_time_type(ElapsedTimeType p_elapsed_type) {
	switch (p_elapsed_type) {
		case ELAPSED_PHYSICS_TIME:
			return PLAY_PHYSICS;
		case ELAPSED_PHYSICS_SCALED_TIME:
			return PLAY_PHYSICS_SCALED;
		case ELAPSED_CLOCK_TIME:
			return APP_CLOCK;
		case ELAPSED_CLOCK_SCALED_TIME:
			return APP_CLOCK_SCALED;
		case ELAPSED_RENDER_TIME:
			return PLAY_RENDER;
		case ELAPSED_RENDER_SCALED_TIME:
			return PLAY_RENDER_SCALED;
		case PHYSICS_FRAME_COUNT:
			return PLAY_PHYSICS_FRAME_COUNT;
		case RENDER_FRAME_COUNT:
			return PLAY_RENDER_FRAME_COUNT;
		default:
			ENSURE(false,
				   "Invalid ElapsedTimeType: " + itos((int)p_elapsed_type));
			return PLAY_PHYSICS;
	}
}

} // namespace godot