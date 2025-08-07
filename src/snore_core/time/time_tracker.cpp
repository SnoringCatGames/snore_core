#include "snore_core/time/time_tracker.h"

#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void TimeTracker::_ready() {
	start_clock_time = Time::get_singleton()->get_ticks_usec() / 1000000.0;
}

void TimeTracker::_process(double p_delta) {
	render_frame_count += 1;
	elapsed_render_time += static_cast<float>(p_delta);
	elapsed_render_scaled_time += static_cast<float>(p_delta) * time_scale;
}

void TimeTracker::_physics_process(double p_delta) {
	physics_frame_count += 1;
	elapsed_physics_time += static_cast<float>(p_delta);
	elapsed_physics_scaled_time += static_cast<float>(p_delta) * time_scale;
	_update_clock_time();
}

void TimeTracker::_update_clock_time() {
	const float next_elapsed_clock_time =
			Time::get_singleton()->get_ticks_usec() / 1000000.0 -
			start_clock_time;
	const float delta_clock_time = next_elapsed_clock_time - elapsed_clock_time;
	elapsed_clock_time = next_elapsed_clock_time;
	elapsed_clock_scaled_time += delta_clock_time * time_scale;
}
