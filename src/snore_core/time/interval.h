#ifndef INTERVAL_H
#define INTERVAL_H

#include "snore_core/time/time_tracker.h"
#include "snore_core/time/time_type.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>

namespace godot {

class Callable;

// Represents a repeating callback scheduled at regular intervals.
class Interval : public RefCounted {
	GDCLASS(Interval, RefCounted)

public:
	Interval() = default;
	~Interval() = default;

	void initialize(
			Object *p_parent,
			TimeType p_time_type,
			const Callable &p_callback,
			float p_interval,
			const Array &p_arguments);

	bool get_has_reached_next_trigger_time() const;

	void trigger();

	int get_id() const { return id; }
	Object *get_parent() const { return parent; }

protected:
	static void _bind_methods() {}

private:
	TimeTracker *time_tracker = nullptr;
	ElapsedTimeType elapsed_time_type = ElapsedTimeType::ELAPSED_PHYSICS_TIME;
	Callable callback;
	float interval = 0.0;
	Array arguments;
	float next_trigger_time = 0.0;
	int id = 0;
	Object *parent = nullptr;
};

} // namespace godot

#endif // INTERVAL_H
