#include "snore_core/time/interval.h"

#include "snore_core/time/elapsed_time_type.h"
#include "snore_core/time/time_service.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>

using namespace godot;

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

void Interval::initialize(
		Object *p_parent,
		TimeType p_time_type,
		const Callable &p_callback,
		float p_interval,
		const Array &p_arguments) {
	parent = p_parent;

	time_tracker =
			TimeService::get()->get_time_tracker_for_time_type(p_time_type);
	elapsed_time_type = TimeService::get()->get_elapsed_time_type_for_time_type(
			p_time_type);
	id = TimeService::get()->get_next_task_id();

	if (time_tracker) {
		float current_time = TimeService::get()->get_elapsed_time(p_time_type);
		next_trigger_time = current_time + p_interval;
	} else {
		next_trigger_time = p_interval;
	}

	callback = p_callback;
	interval = p_interval;
	arguments = p_arguments;
}

bool Interval::get_has_reached_next_trigger_time() const {
	float elapsed_time = TimeService::get()->get_elapsed_time(
			get_time_type_from_elapsed_time_type(elapsed_time_type));
	return elapsed_time >= next_trigger_time;
}

void Interval::trigger() {
	if (!callback.is_valid()) {
		return;
	}

	float current_time = TimeService::get()->get_elapsed_time(
			get_time_type_from_elapsed_time_type(elapsed_time_type));
	next_trigger_time = current_time + interval;

	callback.callv(arguments);
}
