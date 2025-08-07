#include "snore_core/time/interval.h"

#include "snore_core/time/time_service.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

Interval::Interval() {
	time_service = nullptr;
	time_tracker = nullptr;
	interval = 0.0;
	next_trigger_time = 0.0;
	id = 0;
	parent = nullptr;
}

Interval::~Interval() {
	// Destructor implementation.
}

void Interval::initialize(
		TimeService *p_time_service,
		Object *p_parent,
		int p_time_type,
		const Callable &p_callback,
		float p_interval,
		const Array &p_arguments) {
	time_service = p_time_service;
	parent = p_parent;

	if (time_service) {
		time_tracker =
				time_service->get_time_tracker_for_time_type(p_time_type);
		elapsed_time_key =
				time_service->get_elapsed_time_type_for_time_type(p_time_type);
		id = time_service->get_next_task_id();

		if (time_tracker) {
			float current_time = time_service->get_elapsed_time(p_time_type);
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

bool Interval::get_has_reached_next_trigger_time() const {
	if (!time_service || !time_tracker) {
		return false; // Cannot check trigger time without proper setup
	}

	float elapsed_time = time_service->get_elapsed_time(
			time_service->get_time_type_from_elapsed_time_type(
					elapsed_time_key));
	return elapsed_time >= next_trigger_time;
}

void Interval::trigger() {
	if (!callback.is_valid()) {
		return;
	}

	if (time_service && time_tracker) {
		float current_time = time_service->get_elapsed_time(
				time_service->get_time_type_from_elapsed_time_type(
						elapsed_time_key));
		next_trigger_time = current_time + interval;
	} else {
		next_trigger_time += interval;
	}

	callback.callv(arguments);
}

void Interval::_bind_methods() {
	ClassDB::bind_method(
			D_METHOD(
					"initialize", "time_service", "parent", "time_type",
					"callback", "interval", "arguments"),
			&Interval::initialize);
	ClassDB::bind_method(
			D_METHOD("get_has_reached_next_trigger_time"),
			&Interval::get_has_reached_next_trigger_time);
	ClassDB::bind_method(D_METHOD("trigger"), &Interval::trigger);
	ClassDB::bind_method(D_METHOD("get_id"), &Interval::get_id);
	ClassDB::bind_method(D_METHOD("get_parent"), &Interval::get_parent);

	ADD_PROPERTY(
			PropertyInfo(
					Variant::INT, "id", PROPERTY_HINT_NONE, "",
					PROPERTY_USAGE_EDITOR),
			"", "get_id");
}
