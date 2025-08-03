#ifndef TIME_DEBOUNCER_H
#define TIME_DEBOUNCER_H

#include "snore_core/time/time_tracker.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>

// Forward declaration to avoid circular dependency
class TimeService;

namespace godot {

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

// Debounces callback execution to prevent rapid successive calls.
class TimeDebouncer : public RefCounted {
	GDCLASS(TimeDebouncer, RefCounted)

private:
	int time_type;
	TimeService *time_service;
	TimeTracker *time_tracker;
	StringName elapsed_time_key;
	Callable callback;
	float interval;
	bool invokes_at_start;
	Object *parent;

	int last_timeout_id;
	float last_call_time;
	bool is_callback_scheduled;

	// Triggers the debounced callback.
	void _trigger_callback();

public:
	TimeDebouncer();
	~TimeDebouncer();

	// Initializes the debouncer.
	void initialize(
			Object *p_parent,
			TimeService *p_time_service,
			int p_time_type,
			const Callable &p_callback,
			float p_interval,
			bool p_invokes_at_start);

	// The callable that should be used to trigger the debounced execution.
	Callable get_on_call() const;

	// Called when the debounced function should be executed.
	void on_call();

	// Cancels any pending debounced callback.
	void cancel();

	// Gets the parent object.
	Object *get_parent() const { return parent; }

protected:
	static void _bind_methods();
};

} // namespace godot

#endif // TIME_DEBOUNCER_H
