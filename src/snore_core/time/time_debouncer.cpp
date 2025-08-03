#include "snore_core/time/debouncer.h"

#include "snore_core/time/time_service.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

TimeDebouncer::TimeDebouncer() {
	time_type = 0;
	time_service = nullptr;
	time_tracker = nullptr;
	interval = 0.0;
	invokes_at_start = false;
	parent = nullptr;
	last_timeout_id = -1;
	last_call_time = -infinity;
	is_callback_scheduled = false;
}

TimeDebouncer::~TimeDebouncer() {
	// Destructor implementation.
}

void TimeDebouncer::initialize(
		Object *p_parent,
		TimeService *p_time_service,
		int p_time_type,
		const Callable &p_callback,
		float p_interval,
		bool p_invokes_at_start) {
	parent = p_parent;
	time_service = p_time_service;
	time_type = p_time_type;
	if (time_service) {
		time_tracker =
				time_service->_get_time_tracker_for_time_type(p_time_type);
		elapsed_time_key =
				time_service->_get_elapsed_time_key_for_time_type(p_time_type);
	} else {
		time_tracker = nullptr;
		elapsed_time_key = "elapsed_physics_time"; // Fallback
	}
	callback = p_callback;
	interval = p_interval;
	invokes_at_start = p_invokes_at_start;
}

Callable TimeDebouncer::get_on_call() const {
	return callable_mp(
			const_cast<TimeDebouncer *>(this), &TimeDebouncer::on_call);
}

void TimeDebouncer::on_call() {
	if (!time_service || !time_tracker) {
		// Fallback behavior - just trigger immediately
		_trigger_callback();
		return;
	}

	float current_call_time = time_service->get_elapsed_time(time_type);

	if (invokes_at_start && !is_callback_scheduled &&
		current_call_time > last_call_time + interval) {
		_trigger_callback();
		return;
	}

	time_service->clear_timeout(last_timeout_id);
	last_timeout_id = time_service->set_timeout(
			callable_mp(this, &TimeDebouncer::_trigger_callback), interval,
			Array(), time_type);
	is_callback_scheduled = true;
}

void TimeDebouncer::cancel() {
	if (time_service) {
		time_service->clear_timeout(last_timeout_id);
	}
	is_callback_scheduled = false;
}

void TimeDebouncer::_trigger_callback() {
	if (time_service) {
		time_service->clear_timeout(last_timeout_id);
	}

	if (time_service && time_tracker) {
		last_call_time = time_service->get_elapsed_time(time_type);
	}
	is_callback_scheduled = false;
	if (callback.is_valid()) {
		callback.call();
	}
}

void TimeDebouncer::_bind_methods() {
	ClassDB::bind_method(
			D_METHOD(
					"initialize", "parent", "time_service", "time_type",
					"callback", "interval", "invokes_at_start"),
			&TimeDebouncer::initialize);
	ClassDB::bind_method(D_METHOD("get_on_call"), &TimeDebouncer::get_on_call);
	ClassDB::bind_method(D_METHOD("on_call"), &TimeDebouncer::on_call);
	ClassDB::bind_method(D_METHOD("cancel"), &TimeDebouncer::cancel);
	ClassDB::bind_method(D_METHOD("get_parent"), &TimeDebouncer::get_parent);
}
