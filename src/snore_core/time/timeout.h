#ifndef TIMEOUT_H
#define TIMEOUT_H

#include "snore_core/time/elapsed_time_type.h"
#include "snore_core/time/time_type.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

class Callable;
class TimeTracker;

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

// Represents a one-shot callback scheduled for a specific delay.
class Timeout : public RefCounted {
	GDCLASS(Timeout, RefCounted)

public:
	Timeout() = default;
	~Timeout() = default;

	void initialize(
			Object *p_parent,
			TimeType p_time_type,
			const Callable &p_callback,
			float p_delay_sec,
			const Array &p_arguments);

	bool get_has_expired() const;

	void trigger();

	int get_id() const { return id; }
	Object *get_parent() const { return parent; }

protected:
	static void _bind_methods() {}

private:
	TimeTracker *time_tracker = nullptr;
	ElapsedTimeType elapsed_time_type = ElapsedTimeType::ELAPSED_PHYSICS_TIME;
	Callable callback;
	float time = 0.0;
	Array arguments;
	int id = 0;
	Object *parent = nullptr;
};

} // namespace godot

#endif // TIMEOUT_H
