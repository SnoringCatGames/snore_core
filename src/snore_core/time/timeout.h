#ifndef TIMEOUT_H
#define TIMEOUT_H

#include "snore_core/time/time_tracker.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>

namespace godot {

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

// Represents a one-shot callback scheduled for a specific time.
class Timeout : public RefCounted {
	GDCLASS(Timeout, RefCounted)

private:
	TimeTracker *time_tracker;
	StringName elapsed_time_key;
	Callable callback;
	float time;
	Array arguments;
	int id;
	Object *parent;

public:
	Timeout();
	~Timeout();

	// Initializes the timeout.
	void initialize(
			Object *p_parent,
			int p_time_type,
			const Callable &p_callback,
			float p_delay,
			const Array &p_arguments);

	// Checks if the timeout has expired.
	bool get_has_expired() const;

	// Triggers the timeout callback.
	void trigger();

	// Gets the timeout ID.
	int get_id() const { return id; }
	// Gets the parent object.
	Object *get_parent() const { return parent; }

protected:
	static void _bind_methods();
};

} // namespace godot

#endif // TIMEOUT_H
