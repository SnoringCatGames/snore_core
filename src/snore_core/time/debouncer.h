#ifndef DEBOUNCER_H
#define DEBOUNCER_H

#include "snore_core/time/time_tracker.h"
#include "snore_core/time/time_type.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

class Callable;

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

// Debounces callback execution to prevent rapid successive calls.
class Debouncer : public RefCounted {
	GDCLASS(Debouncer, RefCounted)

public:
	Debouncer() = default;
	~Debouncer() = default;

	void initialize(
			Object *p_parent,
			TimeType p_time_type,
			const Callable &p_callback,
			float p_interval,
			bool p_invokes_at_start);

	Callable get_on_call() const;

	void on_call();

	void cancel();

	Object *get_parent() const { return parent; }

protected:
	static void _bind_methods();

private:
	TimeType time_type = TimeType::APP_PHYSICS;
	TimeTracker *time_tracker = nullptr;
	ElapsedTimeType elapsed_time_type = ElapsedTimeType::ELAPSED_PHYSICS_TIME;
	Callable callback;
	float interval = 0.0;
	bool invokes_at_start = false;
	Object *parent = nullptr;

	int last_timeout_id = -1;
	float last_call_time = -infinity;
	bool is_callback_scheduled = false;

	void trigger_callback();
};

} // namespace godot

#endif // DEBOUNCER_H
