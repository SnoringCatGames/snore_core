#ifndef TIME_INTERVAL_H
#define TIME_INTERVAL_H

#include "snore_core/time/time_tracker.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>

namespace godot {

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

// Forward declaration to avoid circular dependency
class SnoreCoreTime;

// Represents a repeating callback scheduled at regular intervals.
class TimeInterval : public RefCounted {
	GDCLASS(TimeInterval, RefCounted)

private:
	SnoreCoreTime *snore_core_time;
	TimeTracker *time_tracker;
	StringName elapsed_time_key;
	Callable callback;
	float interval;
	Array arguments;
	float next_trigger_time;
	int id;
	Object *parent;

public:
	TimeInterval();
	~TimeInterval();

	// Initializes the interval.
	void initialize(
			SnoreCoreTime *p_snore_core_time,
			Object *p_parent,
			int p_time_type,
			const Callable &p_callback,
			float p_interval,
			const Array &p_arguments);

	// Checks if the interval has reached its next trigger time.
	bool get_has_reached_next_trigger_time() const;

	// Triggers the interval callback and schedules the next trigger.
	void trigger();

	// Gets the interval ID.
	int get_id() const { return id; }
	// Gets the parent object.
	Object *get_parent() const { return parent; }

protected:
	static void _bind_methods();
};

} // namespace godot

#endif // TIME_INTERVAL_H
