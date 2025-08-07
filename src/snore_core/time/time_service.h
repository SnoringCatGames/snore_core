#ifndef TIME_SERVICE_H
#define TIME_SERVICE_H

#include "snore_core/snore_core_main_module.h"
#include "snore_core/snore_core_submodule.h"
#include "snore_core/time/ease_type.h"
#include "snore_core/time/elapsed_time_type.h"
#include "snore_core/time/time_service_node.h"
#include "snore_core/time/time_type.h"

#include <godot_cpp/core/class_db.hpp>

namespace godot {

class Callable;
class Debouncer;
class Dictionary;
class Interval;
class SnoreCoreTween;
class Throttler;
class Timeout;
class TimeTracker;

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
class TimeService : public SnoreCoreSubmoduleWithNode {
	GDCLASS(TimeService, SnoreCoreSubmoduleWithNode)
	SC_SUBMODULE_WITH_NODE_CLASS(
			TimeService,
			SnoreCore,
			"TimeServiceProxy",
			TimeServiceNode)

public:
	static const float physics_fps;
	static const float physics_time_step;
	static const float default_time_scale;

	static float play_time();
	static float scaled_play_time();

	TimeService() = default;
	~TimeService() = default;

	void handle_frame(double p_delta);

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
	float get_elapsed_time(TimeType p_time_type) const;

	// Gets the time tracker for a specific time type.
	TimeTracker *get_time_tracker_for_time_type(TimeType p_time_type) const;
	// Gets the elapsed time key for a specific time type.
	ElapsedTimeType get_elapsed_time_type_for_time_type(
			TimeType p_time_type) const;
	// Gets a time type that uses the given elapsed time key.
	int get_time_type_from_elapsed_time_type(
			ElapsedTimeType p_elapsed_key) const;

	// Gets the combined time scale.
	float get_combined_scale() const;
	// Scales a duration by the combined time scale.
	float scale_delta(float p_duration_sec) const;
	// Gets the scaled time step.
	float get_scaled_time_step() const;

	// Gets the time scale.
	float get_time_scale() const { return time_scale; }
	// Sets the time scale.
	void set_time_scale(float p_value);

	// Gets the additional debug time scale.
	float get_additional_debug_time_scale() const {
		return additional_debug_time_scale;
	}
	// Sets the additional debug time scale.
	void set_additional_debug_time_scale(float p_value);

	// Tweens a method call over time.
	int tween_method(
			Object *p_object,
			const StringName &p_key,
			const Variant &p_initial_val,
			const Variant &p_final_val,
			float p_duration_sec,
			EaseType p_ease_type = EaseType::EASE_IN_OUT,
			float p_delay_sec = 0.0,
			TimeType p_time_type = TimeType::APP_PHYSICS,
			const Callable &p_on_completed_callback = Callable(),
			const Array &p_arguments = Array());

	// Tweens a property over time.
	int tween_property(
			Object *p_object,
			const StringName &p_key,
			const Variant &p_initial_val,
			const Variant &p_final_val,
			float p_duration_sec,
			EaseType p_ease_type = EaseType::EASE_IN_OUT,
			float p_delay_sec = 0.0,
			TimeType p_time_type = TimeType::APP_PHYSICS,
			const Callable &p_on_completed_callback = Callable(),
			const Array &p_arguments = Array());

	// Internal tween implementation.
	int tween(
			Object *p_object,
			const StringName &p_key,
			bool p_is_property,
			const Variant &p_initial_val,
			const Variant &p_final_val,
			float p_duration_sec,
			EaseType p_ease_type,
			float p_delay_sec,
			TimeType p_time_type,
			const Callable &p_on_completed_callback,
			const Array &p_arguments);

	// Callback for tween completion.
	void call_tween_completed_callback(
			const Callable &p_on_completed_callback,
			const Array &p_arguments);

	// Clears a tween.
	bool clear_tween(int p_tween_id, bool p_triggers_completed = false);

	// Sets a timeout.
	int set_timeout(
			const Callable &p_callback,
			float p_delay_sec,
			const Array &p_arguments = Array(),
			TimeType p_time_type = TimeType::APP_PHYSICS);

	// Clears a timeout.
	bool clear_timeout(int p_timeout_id, bool p_triggers_timeout = false);

	// Sets an interval.
	int set_interval(
			const Callable &p_callback,
			float p_period,
			const Array &p_arguments = Array(),
			TimeType p_time_type = TimeType::APP_PHYSICS);

	// Clears an interval.
	bool clear_interval(int p_interval_id, bool p_triggers_interval = false);

	// Creates a throttled callback.
	Callable throttle(
			const Callable &p_callback,
			float p_interval,
			bool p_invokes_at_end = true,
			TimeType p_time_type = TimeType::APP_PHYSICS);

	// Clears a throttled callback.
	bool clear_throttle(const Callable &p_throttled_callback);

	// Creates a debounced callback.
	Callable debounce(
			const Callable &p_callback,
			float p_interval,
			bool p_invokes_at_start = false,
			TimeType p_time_type = TimeType::APP_PHYSICS);

	// Clears a debounced callback.
	bool clear_debounce(const Callable &p_debounced_callback);

protected:
	static void _bind_methods();

private:
	static const float default_additional_debug_time_scale;
	static const float garbage_collection_interval;

	float time_scale = default_time_scale;
	float additional_debug_time_scale = default_additional_debug_time_scale;

	int last_timeout_id = -1;

	TimeTracker *app_time_tracker = nullptr;
	TimeTracker *play_time_tracker = nullptr;

	std::unordered_map<int, Timeout *> timeouts;
	std::unordered_map<int, Interval *> intervals;
	std::unordered_map<int, SnoreCoreTween *> tweens;
	std::unordered_map<Callable, Throttler *> throttled_callbacks;
	std::unordered_map<Callable, Debouncer *> debounced_callbacks;

	// Handles tween processing.
	void handle_tweens();
	// Handles timeout processing.
	void handle_timeouts();
	// Handles interval processing.
	void handle_intervals();
};

} // namespace godot

#endif // TIME_SERVICE_H
