#include "snore_core/time/timeout.h"

#include "snore_core/time/time_service.h"
#include "snore_core/time/time_tracker.h"

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Timeout::initialize(
		Object *p_parent,
		TimeType p_time_type,
		const Callable &p_callback,
		float p_delay_sec,
		const Array &p_arguments) {
	parent = p_parent;

	time_tracker =
			TimeService::get()->get_time_tracker_for_time_type(p_time_type);
	elapsed_time_type = TimeService::get()->get_elapsed_time_type_for_time_type(
			p_time_type);
	id = TimeService::get()->get_next_task_id();

	const float current_time =
			TimeService::get()->get_elapsed_time(p_time_type);
	time = current_time + p_delay_sec;

	callback = p_callback;
	arguments = p_arguments;
}

bool Timeout::get_has_expired() const {
	const float elapsed_time = TimeService::get()->get_elapsed_time(
			get_time_type_from_elapsed_time_type(elapsed_time_type));
	return elapsed_time >= time;
}

void Timeout::trigger() {
	if (!ENSURE_SIMPLE(callback.is_valid())) {
		return;
	}
	callback.callv(arguments);
}
