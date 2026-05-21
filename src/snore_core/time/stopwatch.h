#ifndef SC_STOPWATCH_H
#define SC_STOPWATCH_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/string_name.hpp>

namespace godot {

class Stopwatch : public RefCounted {
	GDCLASS(Stopwatch, RefCounted)

protected:
	static void _bind_methods();

public:
	Stopwatch() = default;
	~Stopwatch() = default;

	void start(const StringName &p_metric_key);

	// Returns the elapsed time in milliseconds since the stopwatch was started
	// for the given metric key.
	double stop(const StringName &p_metric_key);

private:
	HashMap<StringName, int64_t> _start_times_usec;
};

} //namespace godot

#endif // SC_STOPWATCH_H
