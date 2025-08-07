#include "snore_core/time/timeout.h"

#include "snore_core/time/time_service.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

Timeout::Timeout() {
	time_service = nullptr;
	time_tracker = nullptr;
	time = 0.0;
	id = 0;
	parent = nullptr;
}

Timeout::~Timeout() {
	// Destructor implementation.
}

void Timeout::initialize(
		TimeService *p_time_service,
		Object *p_parent,
		int p_time_type,
		const Callable &p_callback,
		float p_delay,
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

bool Timeout::get_has_expired() const {
	if (!time_service || !time_tracker) {
		return false; // Cannot check expiration without proper setup
	}

	float elapsed_time = time_service->get_elapsed_time(
			time_service->get_time_type_from_elapsed_time_type(
					elapsed_time_key));
	return elapsed_time >= time;
}

void Timeout::trigger() {
	if (!callback.is_valid()) {
		return;
	}
	callback.callv(arguments);
}

void Timeout::_bind_methods() {
	ClassDB::bind_method(
			D_METHOD(
					"initialize", "time_service", "parent", "time_type",
					"callback", "delay", "arguments"),
			&Timeout::initialize);
	ClassDB::bind_method(
			D_METHOD("get_has_expired"), &Timeout::get_has_expired);
	ClassDB::bind_method(D_METHOD("trigger"), &Timeout::trigger);
	ClassDB::bind_method(D_METHOD("get_id"), &Timeout::get_id);
	ClassDB::bind_method(D_METHOD("get_parent"), &Timeout::get_parent);

	ADD_PROPERTY(
			PropertyInfo(
					Variant::INT, "id", PROPERTY_HINT_NONE, "",
					PROPERTY_USAGE_EDITOR),
			"", "get_id");
}
