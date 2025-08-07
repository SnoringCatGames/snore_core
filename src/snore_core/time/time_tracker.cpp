#include "snore_core/time/time_tracker.h"

#include <godot_cpp/classes/time_service.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

TimeTracker::TimeTracker() {
	time_scale = TimeService::default_time_scale;
	start_clock_time = 0.0;
	elapsed_clock_time = 0.0;
	elapsed_physics_time = 0.0;
	elapsed_render_time = 0.0;
	elapsed_clock_scaled_time = 0.0;
	elapsed_physics_scaled_time = 0.0;
	elapsed_render_scaled_time = 0.0;
	physics_frame_count = 0;
	render_frame_count = 0;
}

TimeTracker::~TimeTracker() {
	// Destructor implementation.
}

void TimeTracker::_ready() {
	start_clock_time = Time::get_singleton()->get_ticks_usec() / 1000000.0;
	elapsed_clock_time = 0.0;
	elapsed_physics_time = 0.0;
	elapsed_render_time = 0.0;
	elapsed_clock_scaled_time = 0.0;
	elapsed_physics_scaled_time = 0.0;
	elapsed_render_scaled_time = 0.0;
	physics_frame_count = 0;
	render_frame_count = 0;
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
	float next_elapsed_clock_time =
			Time::get_singleton()->get_ticks_usec() / 1000000.0 -
			start_clock_time;
	float delta_clock_time = next_elapsed_clock_time - elapsed_clock_time;
	elapsed_clock_time = next_elapsed_clock_time;
	elapsed_clock_scaled_time += delta_clock_time * time_scale;
}

void TimeTracker::_bind_methods() {
	ClassDB::bind_method(
			D_METHOD("get_time_scale"), &TimeTracker::get_time_scale);
	ClassDB::bind_method(
			D_METHOD("set_time_scale", "time_scale"),
			&TimeTracker::set_time_scale);

	ClassDB::bind_method(
			D_METHOD("get_start_clock_time"),
			&TimeTracker::get_start_clock_time);

	ClassDB::bind_method(
			D_METHOD("get_elapsed_clock_time"),
			&TimeTracker::get_elapsed_clock_time);
	ClassDB::bind_method(
			D_METHOD("get_elapsed_physics_time"),
			&TimeTracker::get_elapsed_physics_time);
	ClassDB::bind_method(
			D_METHOD("get_elapsed_render_time"),
			&TimeTracker::get_elapsed_render_time);

	ClassDB::bind_method(
			D_METHOD("get_elapsed_clock_scaled_time"),
			&TimeTracker::get_elapsed_clock_scaled_time);
	ClassDB::bind_method(
			D_METHOD("get_elapsed_physics_scaled_time"),
			&TimeTracker::get_elapsed_physics_scaled_time);
	ClassDB::bind_method(
			D_METHOD("get_elapsed_render_scaled_time"),
			&TimeTracker::get_elapsed_render_scaled_time);

	ClassDB::bind_method(
			D_METHOD("get_physics_frame_count"),
			&TimeTracker::get_physics_frame_count);
	ClassDB::bind_method(
			D_METHOD("get_render_frame_count"),
			&TimeTracker::get_render_frame_count);

	ADD_PROPERTY(
			PropertyInfo(Variant::FLOAT, "time_scale"), "set_time_scale",
			"get_time_scale");
	ADD_PROPERTY(
			PropertyInfo(
					Variant::FLOAT, "start_clock_time", PROPERTY_HINT_NONE, "",
					PROPERTY_USAGE_EDITOR),
			"", "get_start_clock_time");
	ADD_PROPERTY(
			PropertyInfo(
					Variant::FLOAT, "elapsed_clock_time", PROPERTY_HINT_NONE,
					"", PROPERTY_USAGE_EDITOR),
			"", "get_elapsed_clock_time");
	ADD_PROPERTY(
			PropertyInfo(
					Variant::FLOAT, "elapsed_physics_time", PROPERTY_HINT_NONE,
					"", PROPERTY_USAGE_EDITOR),
			"", "get_elapsed_physics_time");
	ADD_PROPERTY(
			PropertyInfo(
					Variant::FLOAT, "elapsed_render_time", PROPERTY_HINT_NONE,
					"", PROPERTY_USAGE_EDITOR),
			"", "get_elapsed_render_time");
	ADD_PROPERTY(
			PropertyInfo(
					Variant::FLOAT, "elapsed_clock_scaled_time",
					PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR),
			"", "get_elapsed_clock_scaled_time");
	ADD_PROPERTY(
			PropertyInfo(
					Variant::FLOAT, "elapsed_physics_scaled_time",
					PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR),
			"", "get_elapsed_physics_scaled_time");
	ADD_PROPERTY(
			PropertyInfo(
					Variant::FLOAT, "elapsed_render_scaled_time",
					PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR),
			"", "get_elapsed_render_scaled_time");
	ADD_PROPERTY(
			PropertyInfo(
					Variant::INT, "physics_frame_count", PROPERTY_HINT_NONE, "",
					PROPERTY_USAGE_EDITOR),
			"", "get_physics_frame_count");
	ADD_PROPERTY(
			PropertyInfo(
					Variant::INT, "render_frame_count", PROPERTY_HINT_NONE, "",
					PROPERTY_USAGE_EDITOR),
			"", "get_render_frame_count");
}
