#ifndef TIME_SERVICE_H
#define TIME_SERVICE_H

#include "snore_core/snore_core_main_module.h"
#include "snore_core/snore_core_submodule.h"
#include "snore_core/time/time_interval.h"
#include "snore_core/time/time_timeout.h"
#include "snore_core/time/time_tracker.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/dictionary.hpp>

namespace godot {

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

// -   This supports tracking many different modes of time:
//     -   App time vs play (unpaused) time
//     -   Wall-clock time vs physics time vs render time
//     -   Scaled (e.g., slow-motion) time vs unscaled time
// -   This also supports many different convenience methods for common
//     scheduling tasks:
//     -   set_timeout: One-shot callback scheduling (just like in JavaScript).
//     -   set_interval Repeated callback scheduling (just like in JavaScript).
//     -   tween_method / tween_property:
//         -   Less boilerplate than Godot's normal Tweens.
//         -   Support configuring which time modes are tracked.
//     -   throttle: Throttling a callback.
//     -   debounce: Debouncing a callback.
// -   This has a built-in garbage-collection mechanism, to clean-up stale
//     timeouts, intervals, tweens, throttles, and debounces.
// -   For an high-level description of this time-tracking, check-out this
// post:
// https://devlog.levi.dev/2021/05/wibbly-wobbly-timey-wimey-tracking-time_service.html
class TimeService : public SnoreCoreSubmodule {
	GDCLASS(TimeService, SnoreCoreSubmodule)
	SC_SUBMODULE_CLASS(TimeService, SnoreCore)

public:
	static const float PHYSICS_FPS;
	static const float PHYSICS_TIME_STEP;
	static const float DEFAULT_TIME_SCALE;

private:
	static const float DEFAULT_ADDITIONAL_DEBUG_TIME_SCALE;
	static const float GARBAGE_COLLECTION_INTERVAL;

	float time_scale;
	float additional_debug_time_scale;

	TimeTracker *_app_time;
	TimeTracker *_play_time;

	// Dictionary<int, TimeTimeout*>
	Dictionary _timeouts;
	// Dictionary<int, TimeInterval*>
	Dictionary _intervals;
	// Dictionary<int, ScaffolderTween*>
	Dictionary _tweens;
	int _last_timeout_id;
	// Dictionary<Callable, TimeThrottler*>
	Dictionary _throttled_callbacks;
	// Dictionary<Callable, TimeDebouncer*>
	Dictionary _debounced_callbacks;

	// Handles tween processing.
	void _handle_tweens();
	// Handles timeout processing.
	void _handle_timeouts();
	// Handles interval processing.
	void _handle_intervals();

public:
	static float play_time();
	static float scaled_play_time();

	TimeService();
	~TimeService();

	void _init() override;
	void _ready() override;
	void _process(double p_delta) override;

	// Cleans up stale timeouts, intervals, tweens, throttles, and debounces.
	void collect_garbage();

	// Gets the next unique task ID.
	int get_next_task_id();

	// Gets the app time (total time since app started).
	float get_app_time() const;
	// Gets the clock time (real-world time since app started).
	float get_clock_time() const;
	// Gets the play time (unpaused time).
	float get_play_time() const;
	// Gets the scaled play time.
	float get_scaled_play_time() const;

	// Gets the play physics frame count.
	int get_play_physics_frame_count() const;

	// Gets elapsed time for a specific time type.
	float get_elapsed_time(int p_time_type) const;

	// Gets the time tracker for a specific time type.
	TimeTracker *_get_time_tracker_for_time_type(int p_time_type) const;
	// Gets the elapsed time key for a specific time type.
	StringName _get_elapsed_time_key_for_time_type(int p_time_type) const;
	// Gets a time type that uses the given elapsed time key.
	int _get_time_type_from_key(
			StringName const StringNameNameName &p_elapsed_time_key) const;

	// Gets the combined time scale.
	float get_combined_scale() const;
	// Scales a duration by the combined time scale.
	float scale_delta(float p_duration) const;
	// Gets the scaled time step.
	float get_scaled_time_step() const;

	// Gets the time scale.
	float get_time_scale() const { return time_scale; }
	// Sets the time scale.
	void _set_time_scale(float p_value);

	// Gets the additional debug time scale.
	float get_additional_debug_time_scale() const {
		return additional_debug_time_scale;
	}
	// Sets the additional debug time scale.
	void _set_additional_debug_time_scale(float p_value);

	// Tweens a method call over time.
	int tween_method(
			Object *p_object,
			const StringNameName &p_key,
			const Variant &p_initial_val,
			const Variant &p_final_val,
			float p_duration,
			const StringName &p_ease_name = "ease_in_out",
			float p_delay = 0.0,
			int p_time_type = 0,
			const Callable &p_on_completed_callback = Callable(),
			const Array &p_arguments = Array());

	// Tweens a property over time.
	int tween_property(
			Object *p_object,
			const StringNameName &p_key,
			const Variant &p_initial_val,
			const Variant &p_final_val,
			float p_duration,
			const StringName &p_ease_name = "ease_in_out",
			float p_delay = 0.0,
			int p_time_type = 0,
			const Callable &p_on_completed_callback = Callable(),
			const Array &p_arguments = Array());

	// Internal tween implementation.
	int _tween(
			Object *p_object,
			const StringName &p_key,
			bool p_is_property,
			const Variant &p_initial_val,
			const Variant &p_final_val,
			float p_duration,
			const StringName &p_ease_name,
			float p_delay,
			int p_time_type,
			const Callable &p_on_completed_callback,
			const Array &p_arguments);

	// Callback for tween completion.
	void _call_tween_completed_callback(
			const Callable &p_on_completed_callback,
			const Array &p_arguments);

	// Clears a tween.
	bool clear_tween(int p_tween_id, bool p_triggers_completed = false);

	// Sets a timeout.
	int set_timeout(
			const Callable &p_callback,
			float p_delay,
			const Array &p_arguments = Array(),
			int p_time_type = 0);

	// Clears a timeout.
	bool clear_timeout(int p_timeout_id, bool p_triggers_timeout = false);

	// Sets an interval.
	int set_interval(
			const Callable &p_callback,
			float p_period,
			const Array &p_arguments = Array(),
			int p_time_type = 0);

	// Clears an interval.
	bool clear_interval(int p_interval_id, bool p_triggers_interval = false);

	// Creates a throttled callback.
	Callable throttle(
			const Callable &p_callback,
			float p_interval,
			bool p_invokes_at_end = true,
			int p_time_type = 0);

	// Clears a throttled callback.
	bool clear_throttle(const Callable &p_throttled_callback);

	// Creates a debounced callback.
	Callable debounce(
			const Callable &p_callback,
			float p_interval,
			bool p_invokes_at_start = false,
			int p_time_type = 0);

	// Clears a debounced callback.
	bool clear_debounce(const Callable &p_debounced_callback);

protected:
	static void _bind_methods();
};

} // namespace godot

#endif // TIME_SERVICE_H
