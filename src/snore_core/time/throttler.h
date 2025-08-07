#ifndef THROTTLER_H
#define THROTTLER_H

#include "snore_core/time/time_tracker.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>

namespace godot {

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

// Throttles callback execution to limit frequency.
class Throttler : public RefCounted {
	GDCLASS(Throttler, RefCounted)

private:
	int time_type;
	TimeTracker *time_tracker;
	StringName elapsed_time_key;
	Callable callback;
	float interval;
	bool invokes_at_end;
	Object *parent;

	int last_timeout_id;
	float last_call_time;
	bool is_callback_scheduled;

	// Triggers the throttled callback.
	void _trigger_callback();

public:
	Throttler();
	~Throttler();

	// Initializes the throttler.
	void initialize(
			Object *p_parent,
			int p_time_type,
			const Callable &p_callback,
			float p_interval,
			bool p_invokes_at_end);

	// The callable that should be used to trigger the throttled execution.
	Callable get_on_call() const;

	// Called when the throttled function should be executed.
	void on_call();

	// Cancels any pending throttled callback.
	void cancel();

	// Gets the parent object.
	Object *get_parent() const { return parent; }

protected:
	static void _bind_methods();
};

} // namespace godot

#endif // THROTTLER_H
