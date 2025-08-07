#include "snore_core/time/throttler.h"

#include "snore_core/time/time_service.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

Throttler::Throttler() {
	time_service = nullptr;
	time_type = 0;
	time_tracker = nullptr;
	interval = 0.0;
	invokes_at_end = true;
	parent = nullptr;
	last_timeout_id = -1;
	last_call_time = -infinity;
	is_callback_scheduled = false;
}

Throttler::~Throttler() {
	// Destructor implementation.
}

void Throttler::initialize(
		TimeService *p_time_service,
		Object *p_parent,
		int p_time_type,
		const Callable &p_callback,
		float p_interval,
		bool p_invokes_at_end) {
	time_service = p_time_service;
	parent = p_parent;
	time_type = p_time_type;

	if (time_service) {
		time_tracker =
				time_service->get_time_tracker_for_time_type(p_time_type);
		elapsed_time_key =
				time_service->get_elapsed_time_type_for_time_type(p_time_type);
	} else {
		time_tracker = nullptr;
		elapsed_time_key = "elapsed_physics_time";
	}

	callback = p_callback;
	interval = p_interval;
	invokes_at_end = p_invokes_at_end;
}

Callable Throttler::get_on_call() const {
	return callable_mp(const_cast<Throttler *>(this), &Throttler::on_call);
}

void Throttler::on_call() {
	if (!is_callback_scheduled) {
		if (!time_service || !time_tracker) {
			// Fallback behavior - just trigger immediately
			_trigger_callback();
			return;
		}

		float current_call_time = time_service->get_elapsed_time(time_type);
		float next_call_time = last_call_time + interval;

		if (current_call_time > next_call_time) {
			_trigger_callback();
		} else if (invokes_at_end) {
			last_timeout_id = time_service->set_timeout(
					callable_mp(this, &Throttler::_trigger_callback),
					next_call_time - current_call_time, Array(), time_type);
			is_callback_scheduled = true;
		}
	}
}

void Throttler::cancel() {
	if (time_service) {
		time_service->clear_timeout(last_timeout_id);
	}
	is_callback_scheduled = false;
}

void Throttler::_trigger_callback() {
	if (time_service && time_tracker) {
		last_call_time = time_service->get_elapsed_time(time_type);
	}
	is_callback_scheduled = false;
	if (callback.is_valid()) {
		callback.call();
	}
}

void Throttler::_bind_methods() {
	ClassDB::bind_method(
			D_METHOD(
					"initialize", "parent", "time_type", "callback", "interval",
					"invokes_at_end"),
			&Throttler::initialize);
	ClassDB::bind_method(D_METHOD("get_on_call"), &Throttler::get_on_call);
	ClassDB::bind_method(D_METHOD("on_call"), &Throttler::on_call);
	ClassDB::bind_method(D_METHOD("cancel"), &Throttler::cancel);
	ClassDB::bind_method(D_METHOD("get_parent"), &Throttler::get_parent);
}
