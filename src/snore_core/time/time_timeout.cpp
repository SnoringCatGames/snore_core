#include "snore_core/time/time_timeout.h"

#include "snore_core/time/snore_core_time.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

TimeTimeout::TimeTimeout() {
	snore_core_time = nullptr;
	time_tracker = nullptr;
	time = 0.0;
	id = 0;
	parent = nullptr;
}

TimeTimeout::~TimeTimeout() {
	// Destructor implementation.
}

void TimeTimeout::initialize(
		SnoreCoreTime *p_snore_core_time,
		Object *p_parent,
		int p_time_type,
		const Callable &p_callback,
		float p_delay,
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
			time = current_time + p_delay;
		} else {
			time = p_delay;
		}
	} else {
		time_tracker = nullptr;
		elapsed_time_key = "elapsed_physics_time";
		time = p_delay;
		static int next_id = 0;
		id = next_id++;
	}

	callback = p_callback;
	arguments = p_arguments;
}

bool TimeTimeout::get_has_expired() const {
	if (!snore_core_time || !time_tracker) {
		return false; // Cannot check expiration without proper setup
	}

	float elapsed_time = snore_core_time->get_elapsed_time(
			snore_core_time->_get_time_type_from_key(elapsed_time_key));
	return elapsed_time >= time;
}

void TimeTimeout::trigger() {
	if (!callback.is_valid()) {
		return;
	}
	callback.callv(arguments);
}

void TimeTimeout::_bind_methods() {
	ClassDB::bind_method(
			D_METHOD(
					"initialize", "snore_core_time", "parent", "time_type",
					"callback", "delay", "arguments"),
			&TimeTimeout::initialize);
	ClassDB::bind_method(
			D_METHOD("get_has_expired"), &TimeTimeout::get_has_expired);
	ClassDB::bind_method(D_METHOD("trigger"), &TimeTimeout::trigger);
	ClassDB::bind_method(D_METHOD("get_id"), &TimeTimeout::get_id);
	ClassDB::bind_method(D_METHOD("get_parent"), &TimeTimeout::get_parent);

	ADD_PROPERTY(
			PropertyInfo(
					Variant::INT, "id", PROPERTY_HINT_NONE, "",
					PROPERTY_USAGE_EDITOR),
			"", "get_id");
}
