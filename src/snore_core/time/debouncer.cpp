#include "snore_core/time/debouncer.h"

#include "snore_core/time/time_service.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>

using namespace godot;

void Debouncer::initialize(
		Object *p_parent,
		TimeType p_time_type,
		const Callable &p_callback,
		float p_interval,
		bool p_invokes_at_start) {
	parent = p_parent;
	time_type = p_time_type;
	time_tracker =
			TimeService::get()->get_time_tracker_for_time_type(p_time_type);
	elapsed_time_type = get_elapsed_time_type_for_time_type(p_time_type);
	callback = p_callback;
	interval = p_interval;
	invokes_at_start = p_invokes_at_start;
	client_callback = callable_mp(
			const_cast<Debouncer *>(this), &Debouncer::trigger_limited_call);
}

void Debouncer::trigger_limited_call() {
	const float current_call_time =
			TimeService::get()->get_elapsed_time(time_type);

	if (invokes_at_start && !is_callback_scheduled &&
		current_call_time > last_call_time + interval) {
		trigger_callback();
		return;
	}

	TimeService::get()->clear_timeout(last_timeout_id);
	last_timeout_id = TimeService::get()->set_timeout(
			callable_mp(this, &Debouncer::trigger_callback), interval, Array(),
			time_type);
	is_callback_scheduled = true;
}

void Debouncer::cancel() {
	TimeService::get()->clear_timeout(last_timeout_id);
	is_callback_scheduled = false;
}

void Debouncer::trigger_callback() {
	TimeService::get()->clear_timeout(last_timeout_id);

	last_call_time = TimeService::get()->get_elapsed_time(time_type);
	is_callback_scheduled = false;
	if (is_valid(callback)) {
		callback.call();
	}
}

void Debouncer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("cancel"), &Debouncer::cancel);
}
