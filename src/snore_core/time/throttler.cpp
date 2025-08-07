#include "snore_core/time/throttler.h"

#include "snore_core/time/time_service.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>

using namespace godot;

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

void Throttler::initialize(
		Object *p_parent,
		TimeType p_time_type,
		const Callable &p_callback,
		float p_interval,
		bool p_invokes_at_end) {
	parent = p_parent;
	time_type = p_time_type;

	time_tracker =
			TimeService::get()->get_time_tracker_for_time_type(p_time_type);
	elapsed_time_type = TimeService::get()->get_elapsed_time_type_for_time_type(
			p_time_type);

	callback = p_callback;
	interval = p_interval;
	invokes_at_end = p_invokes_at_end;
}

Callable Throttler::get_on_call() const {
	return callable_mp(const_cast<Throttler *>(this), &Throttler::on_call);
}

void Throttler::on_call() {
	if (!is_callback_scheduled) {
		float current_call_time =
				TimeService::get()->get_elapsed_time(time_type);
		float next_call_time = last_call_time + interval;

		if (current_call_time > next_call_time) {
			trigger_callback();
		} else if (invokes_at_end) {
			last_timeout_id = TimeService::get()->set_timeout(
					callable_mp(this, &Throttler::trigger_callback),
					next_call_time - current_call_time, Array(), time_type);
			is_callback_scheduled = true;
		}
	}
}

void Throttler::cancel() {
	TimeService::get()->clear_timeout(last_timeout_id);
	is_callback_scheduled = false;
}

void Throttler::trigger_callback() {
	last_call_time = TimeService::get()->get_elapsed_time(time_type);
	is_callback_scheduled = false;
	if (callback.is_valid()) {
		callback.call();
	}
}

void Throttler::_bind_methods() {
	ClassDB::bind_method(D_METHOD("cancel"), &Throttler::cancel);
}
