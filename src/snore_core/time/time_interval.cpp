#include "snore_core/time/time_interval.h"

#include "snore_core/time/snore_core_time.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

TimeInterval::TimeInterval() {
	snore_core_time = nullptr;
	time_tracker = nullptr;
	interval = 0.0;
	next_trigger_time = 0.0;
	id = 0;
	parent = nullptr;
}

TimeInterval::~TimeInterval() {
	// Destructor implementation.
}

void TimeInterval::initialize(
		SnoreCoreTime *p_snore_core_time,
		Object *p_parent,
		int p_time_type,
		const Callable &p_callback,
		float p_interval,
		const Array &p_arguments) {
	snore_core_time = p_snore_core_time;
	parent = p_parent;

	if (snore_core_time) {
		time_tracker =
				snore_core_time->_get_time_tracker_for_time_type(p_time_type);
		elapsed_time_key = snore_core_time->_get_elapsed_time_key_for_time_type(
				p_time_type);
		id = snore_core_time->get_next_task_id();

		if (time_tracker) {
			float current_time = snore_core_time->get_elapsed_time(p_time_type);
			next_trigger_time = current_time + p_interval;
		} else {
			next_trigger_time = p_interval;
		}
	} else {
		time_tracker = nullptr;
		elapsed_time_key = "elapsed_physics_time";
		next_trigger_time = p_interval;
		static int next_id = 0;
		id = next_id++;
	}

	callback = p_callback;
	interval = p_interval;
	arguments = p_arguments;
}

bool TimeInterval::get_has_reached_next_trigger_time() const {
	if (!snore_core_time || !time_tracker) {
		return false; // Cannot check trigger time without proper setup
	}

	float elapsed_time = snore_core_time->get_elapsed_time(
			snore_core_time->_get_time_type_from_key(elapsed_time_key));
	return elapsed_time >= next_trigger_time;
}

void TimeInterval::trigger() {
	if (!callback.is_valid()) {
		return;
	}

	if (snore_core_time && time_tracker) {
		float current_time = snore_core_time->get_elapsed_time(
				snore_core_time->_get_time_type_from_key(elapsed_time_key));
		next_trigger_time = current_time + interval;
	} else {
		next_trigger_time += interval;
	}

	callback.callv(arguments);
}

void TimeInterval::_bind_methods() {
	ClassDB::bind_method(
			D_METHOD(
					"initialize", "snore_core_time", "parent", "time_type",
					"callback", "interval", "arguments"),
			&TimeInterval::initialize);
	ClassDB::bind_method(
			D_METHOD("get_has_reached_next_trigger_time"),
			&TimeInterval::get_has_reached_next_trigger_time);
	ClassDB::bind_method(D_METHOD("trigger"), &TimeInterval::trigger);
	ClassDB::bind_method(D_METHOD("get_id"), &TimeInterval::get_id);
	ClassDB::bind_method(D_METHOD("get_parent"), &TimeInterval::get_parent);

	ADD_PROPERTY(
			PropertyInfo(
					Variant::INT, "id", PROPERTY_HINT_NONE, "",
					PROPERTY_USAGE_EDITOR),
			"", "get_id");
}
