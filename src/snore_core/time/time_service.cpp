#include "snore_core/time/time_service.h"

#include "snore_core/snore_core_main_module.h"
#include "snore_core/snore_core_submodule.h"
#include "snore_core/time/debouncer.h"
#include "snore_core/time/interval.h"
#include "snore_core/time/throttler.h"
#include "snore_core/time/time_service_node.h"
#include "snore_core/time/time_tracker.h"
#include "snore_core/time/time_type.h"
#include "snore_core/time/timeout.h"
#include "snore_core/time/tween.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <unordered_map>

using namespace godot;

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

const float TimeService::physics_fps = 60.0;
const float TimeService::physics_time_step = 1.0 / physics_fps;

const float TimeService::default_time_scale = 1.0;
const float TimeService::default_additional_debug_time_scale = 1.0;
const float TimeService::garbage_collection_interval = 30.0;

float TimeService::play_time() {
	if (TimeService *time = TimeService::get_maybe()) {
		return time->get_play_time();
	}
	return 0.0;
}

float TimeService::scaled_play_time() {
	if (TimeService *time = TimeService::get_maybe()) {
		return time->get_scaled_play_time();
	}
	return 0.0;
}

void TimeService::set_up() {
	app_time_tracker = memnew(TimeTracker);
	app_time_tracker->set_process_mode(Node::ProcessMode::PROCESS_MODE_ALWAYS);
	node->add_child(app_time_tracker);

	play_time_tracker = memnew(TimeTracker);
	play_time_tracker->set_process_mode(
			Node::ProcessMode::PROCESS_MODE_PAUSABLE);
	node->add_child(play_time_tracker);

	set_interval(
			callable_mp(this, &TimeService::collect_garbage),
			garbage_collection_interval);
}

void TimeService::reset() {}

void TimeService::handle_frame(double p_delta) {
	handle_tweens();
	handle_timeouts();
	handle_intervals();
}

void TimeService::handle_tweens() {
	Array finished_tween_ids;
	for (int i = 0; i < tweens.size(); ++i) {
		Variant key = tweens.get_key_list()[i];
		// FIXME: LEFT OFF HERE: Implement ScaffolderTween and proper handling.

		// ScaffolderTween *tween =
		// Object::cast_to<ScaffolderTween>(tweens[key]); if (tween) {
		//     tween->step();
		//     if (!tween->is_active()) {
		//         finished_tween_ids.push_back(key);
		//     }
		// }
	}

	for (int i = 0; i < finished_tween_ids.size(); ++i) {
		tweens.erase(finished_tween_ids[i]);
	}
}

void TimeService::handle_timeouts() {
	int expired_timeout_id = -1;
	for (int i = 0; i < timeouts.size(); ++i) {
		Variant key = timeouts.get_key_list()[i];
		Timeout *timeout = Object::cast_to<Timeout>(timeouts[key]);
		if (timeout && timeout->get_has_expired()) {
			expired_timeout_id = key;
			break;
		}
	}

	if (expired_timeout_id >= 0) {
		Timeout *timeout =
				Object::cast_to<Timeout>(timeouts[expired_timeout_id]);
		if (timeout) {
			timeout->trigger();
		}
		timeouts.erase(expired_timeout_id);
	}
}

void TimeService::handle_intervals() {
	int triggered_interval_id = -1;
	for (int i = 0; i < intervals.size(); ++i) {
		Variant key = intervals.get_key_list()[i];
		Interval *interval = Object::cast_to<Interval>(intervals[key]);
		if (interval && interval->get_has_reached_next_trigger_time()) {
			triggered_interval_id = key;
			break;
		}
	}

	if (triggered_interval_id >= 0) {
		Interval *interval =
				Object::cast_to<Interval>(intervals[triggered_interval_id]);
		if (interval) {
			interval->trigger();
		}
	}
}

void TimeService::collect_garbage() {
	Array collections = Array::make(
			timeouts, intervals, tweens, throttled_callbacks,
			debounced_callbacks);

	for (int i = 0; i < collections.size(); ++i) {
		Dictionary collection = collections[i];
		Array keys = collection.keys();

		for (int j = 0; j < keys.size(); ++j) {
			Variant key = keys[j];
			Object *obj = Object::cast_to<Object>(collection[key]);

			if (!obj || !is_instance_valid(obj)) {
				collection.erase(key);
				continue;
			}

			// Check parent validity for timeout/interval objects
			Timeout *timeout = Object::cast_to<Timeout>(obj);
			Interval *interval = Object::cast_to<Interval>(obj);

			Object *parent = nullptr;
			if (timeout) {
				parent = timeout->get_parent();
			} else if (interval) {
				parent = interval->get_parent();
			}

			if (parent && !is_instance_valid(parent)) {
				if (!Object::cast_to<RefCounted>(obj)) {
					obj->call("free");
				}
				collection.erase(key);
			}
		}
	}
}

int TimeService::get_next_task_id() {
	last_timeout_id += 1;
	return last_timeout_id;
}

float TimeService::get_app_time() const {
	return get_elapsed_time(TimeType::APP_PHYSICS);
}

float TimeService::get_clock_time() const {
	return get_elapsed_time(TimeType::APP_CLOCK);
}

float TimeService::get_play_time() const {
	return get_elapsed_time(TimeType::PLAY_PHYSICS);
}

float TimeService::get_scaled_play_time() const {
	return get_elapsed_time(TimeType::PLAY_PHYSICS_SCALED);
}

int TimeService::get_play_physics_frame_count() const {
	return play_time_tracker ? play_time_tracker->get_physics_frame_count() : 0;
}

float TimeService::get_elapsed_time(TimeType p_time_type) const {
	const TimeTracker *tracker = get_time_tracker_for_time_type(p_time_type);
	if (!tracker) {
		return 0.0;
	}
	const ElapsedTimeType elapsed_time_type =
			get_elapsed_time_type_for_time_type(p_time_type);

	switch (elapsed_time_type) {
		case ElapsedTimeType::ELAPSED_PHYSICS_TIME:
			return tracker->get_elapsed_physics_time();
		case ElapsedTimeType::ELAPSED_PHYSICS_SCALED_TIME:
			return tracker->get_elapsed_physics_scaled_time();
		case ElapsedTimeType::ELAPSED_CLOCK_TIME:
			return tracker->get_elapsed_clock_time();
		case ElapsedTimeType::ELAPSED_CLOCK_SCALED_TIME:
			return tracker->get_elapsed_clock_scaled_time();
		case ElapsedTimeType::ELAPSED_RENDER_TIME:
			return tracker->get_elapsed_render_time();
		case ElapsedTimeType::ELAPSED_RENDER_SCALED_TIME:
			return tracker->get_elapsed_render_scaled_time();
		case ElapsedTimeType::PHYSICS_FRAME_COUNT:
			return static_cast<float>(tracker->get_physics_frame_count());
		case ElapsedTimeType::RENDER_FRAME_COUNT:
			return static_cast<float>(tracker->get_render_frame_count());
	}

	return 0.0;
}

TimeTracker *TimeService::get_time_tracker_for_time_type(
		TimeType p_time_type) const {
	switch (p_time_type) {
		case TimeType::APP_PHYSICS:
		case TimeType::APP_CLOCK:
		case TimeType::APP_PHYSICS_SCALED:
		case TimeType::APP_CLOCK_SCALED:
		case TimeType::APP_PHYSICS_FRAME_COUNT:
		case TimeType::APP_RENDER_FRAME_COUNT:
			return app_time_tracker;
		case TimeType::PLAY_PHYSICS:
		case TimeType::PLAY_RENDER:
		case TimeType::PLAY_PHYSICS_SCALED:
		case TimeType::PLAY_RENDER_SCALED:
		case TimeType::PLAY_PHYSICS_FRAME_COUNT:
		case TimeType::PLAY_RENDER_FRAME_COUNT:
			return play_time_tracker;
		default:
			// TODO: Implement access to S.log.error().
			// S.log.error("Unrecognized time_type: %d", p_time_type);
			return nullptr;
	}
}

float TimeService::get_combined_scale() const {
	return time_scale * additional_debug_time_scale;
}

float TimeService::scale_delta(float p_duration) const {
	return p_duration * get_combined_scale();
}

float TimeService::get_scaled_time_step() const {
	return physics_time_step * get_combined_scale();
}

void TimeService::set_time_scale(float p_value) {
	time_scale = p_value;
	if (app_time_tracker) {
		app_time_tracker->set_time_scale(get_combined_scale());
	}
	if (play_time_tracker) {
		play_time_tracker->set_time_scale(get_combined_scale());
	}
}

void TimeService::set_additional_debug_time_scale(float p_value) {
	additional_debug_time_scale = p_value;
	if (app_time_tracker) {
		app_time_tracker->set_time_scale(get_combined_scale());
	}
	if (play_time_tracker) {
		play_time_tracker->set_time_scale(get_combined_scale());
	}
}

int TimeService::tween_method(
		Object *p_object,
		const StringName &p_key,
		const Variant &p_initial_val,
		const Variant &p_final_val,
		float p_duration,
		EaseType p_ease_type,
		float p_delay_sec,
		TimeType p_time_type,
		const Callable &p_on_completed_callback,
		const Array &p_arguments) {
	return tween(
			p_object, p_key, false, p_initial_val, p_final_val, p_duration,
			p_ease_type, p_delay_sec, p_time_type, p_on_completed_callback,
			p_arguments);
}

int TimeService::tween_property(
		Object *p_object,
		const StringName &p_key,
		const Variant &p_initial_val,
		const Variant &p_final_val,
		float p_duration,
		EaseType p_ease_type,
		float p_delay_sec,
		TimeType p_time_type,
		const Callable &p_on_completed_callback,
		const Array &p_arguments) {
	return tween(
			p_object, p_key, true, p_initial_val, p_final_val, p_duration,
			p_ease_type, p_delay_sec, p_time_type, p_on_completed_callback,
			p_arguments);
}

int TimeService::tween(
		Object *p_object,
		const StringName &p_key,
		bool p_is_property,
		const Variant &p_initial_val,
		const Variant &p_final_val,
		float p_duration,
		EaseType p_ease_type,
		float p_delay_sec,
		TimeType p_time_type,
		const Callable &p_on_completed_callback,
		const Array &p_arguments) {
	// TODO: Implement ScaffolderTween creation and management.
	// ScaffolderTween *tween = memnew(ScaffolderTween(p_object, false));
	// tween->_interpolate(p_object, p_key, p_is_property, p_initial_val,
	// p_final_val, p_duration, p_ease_type, p_delay_sec, p_time_type); if
	// (p_on_completed_callback.is_valid()) {
	//     tween->connect("tween_all_completed", callable_mp(this,
	//     &TimeService::call_tween_completed_callback).bind(p_on_completed_callback,
	//     p_arguments));
	// }
	// tween->start();
	// tweens[tween->get_id()] = tween;
	// return tween->get_id();
	return -1; // Placeholder
}

void TimeService::call_tween_completed_callback(
		const Callable &p_on_completed_callback,
		const Array &p_arguments) {
	p_on_completed_callback.callv(p_arguments);
}

bool TimeService::clear_tween(int p_tween_id, bool p_triggers_completed) {
	if (!tweens.has(p_tween_id)) {
		return false;
	}
	// TODO: Implement proper tween handling.
	// ScaffolderTween *tween =
	// Object::cast_to<ScaffolderTween>(tweens[p_tween_id]); if
	// (p_triggers_completed && tween) {
	//     tween->trigger_completed();
	// }
	// if (tween) {
	//     tween->free();
	// }
	tweens.erase(p_tween_id);
	return true;
}

int TimeService::set_timeout(
		const Callable &p_callback,
		float p_delay_sec,
		const Array &p_arguments,
		TimeType p_time_type) {
	Timeout *timeout = memnew(Timeout);
	timeout->initialize(
			this, p_callback.get_object(), p_time_type, p_callback, p_delay_sec,
			p_arguments);
	timeouts[timeout->get_id()] = timeout;
	return timeout->get_id();
}

bool TimeService::clear_timeout(int p_timeout_id, bool p_triggers_timeout) {
	if (!timeouts.has(p_timeout_id)) {
		return false;
	}
	if (p_triggers_timeout) {
		Timeout *timeout = Object::cast_to<Timeout>(timeouts[p_timeout_id]);
		if (timeout) {
			timeout->trigger();
		}
	}
	timeouts.erase(p_timeout_id);
	return true;
}

int TimeService::set_interval(
		const Callable &p_callback,
		float p_period,
		const Array &p_arguments,
		TimeType p_time_type) {
	Interval *interval = memnew(Interval);
	interval->initialize(
			this, p_callback.get_object(), p_time_type, p_callback, p_period,
			p_arguments);
	intervals[interval->get_id()] = interval;
	return interval->get_id();
}

bool TimeService::clear_interval(int p_interval_id, bool p_triggers_interval) {
	if (!intervals.has(p_interval_id)) {
		return false;
	}
	if (p_triggers_interval) {
		Interval *interval =
				Object::cast_to<Interval>(intervals[p_interval_id]);
		if (interval) {
			interval->trigger();
		}
	}
	intervals.erase(p_interval_id);
	return true;
}

Callable TimeService::throttle(
		const Callable &p_callback,
		float p_interval,
		bool p_invokes_at_end,
		TimeType p_time_type) {
	Throttler *throttler = memnew(Throttler);
	throttler->initialize(
			p_callback.get_object(), this, p_time_type, p_callback, p_interval,
			p_invokes_at_end);
	throttled_callbacks[throttler->get_on_call()] = throttler;
	return throttler->get_on_call();
}

bool TimeService::clear_throttle(const Callable &p_throttled_callback) {
	if (!throttled_callbacks.has(p_throttled_callback)) {
		return false;
	}
	Throttler *throttler = Object::cast_to<Throttler>(
			throttled_callbacks[p_throttled_callback]);
	if (throttler) {
		throttler->cancel();
	}
	throttled_callbacks.erase(p_throttled_callback);
	return true;
}

Callable TimeService::debounce(
		const Callable &p_callback,
		float p_interval,
		bool p_invokes_at_start,
		TimeType p_time_type) {
	Debouncer *debouncer = memnew(Debouncer);
	debouncer->initialize(
			p_callback.get_object(), this, p_time_type, p_callback, p_interval,
			p_invokes_at_start);
	debounced_callbacks[debouncer->get_on_call()] = debouncer;
	return debouncer->get_on_call();
}

bool TimeService::clear_debounce(const Callable &p_debounced_callback) {
	if (!debounced_callbacks.has(p_debounced_callback)) {
		return false;
	}
	Debouncer *debouncer = Object::cast_to<Debouncer>(
			debounced_callbacks[p_debounced_callback]);
	if (debouncer) {
		debouncer->cancel();
	}
	debounced_callbacks.erase(p_debounced_callback);
	return true;
}

void TimeService::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_app_time"), &TimeService::get_app_time);
	ClassDB::bind_method(
			D_METHOD("get_clock_time"), &TimeService::get_clock_time);
	ClassDB::bind_method(
			D_METHOD("get_play_time"), &TimeService::get_play_time);
	ClassDB::bind_method(
			D_METHOD("get_scaled_play_time"),
			&TimeService::get_scaled_play_time);
	ClassDB::bind_method(
			D_METHOD("get_play_physics_frame_count"),
			&TimeService::get_play_physics_frame_count);

	ClassDB::bind_method(
			D_METHOD("get_elapsed_time", "time_type"),
			&TimeService::get_elapsed_time);

	ClassDB::bind_method(
			D_METHOD("get_combined_scale"), &TimeService::get_combined_scale);
	ClassDB::bind_method(
			D_METHOD("scale_delta", "duration"), &TimeService::scale_delta);
	ClassDB::bind_method(
			D_METHOD("get_scaled_time_step"),
			&TimeService::get_scaled_time_step);

	ClassDB::bind_method(
			D_METHOD("get_time_scale"), &TimeService::get_time_scale);
	ClassDB::bind_method(
			D_METHOD("set_time_scale", "time_scale"),
			&TimeService::set_time_scale);

	ClassDB::bind_method(
			D_METHOD("get_additional_debug_time_scale"),
			&TimeService::get_additional_debug_time_scale);
	ClassDB::bind_method(
			D_METHOD(
					"set_additional_debug_time_scale",
					"additional_debug_time_scale"),
			&TimeService::set_additional_debug_time_scale);

	ClassDB::bind_method(
			D_METHOD(
					"tween_method", "object", "key", "initial_val", "final_val",
					"duration", "ease_name", "delay", "time_type",
					"on_completed_callback", "arguments"),
			&TimeService::tween_method, DEFVAL("ease_in_out"), DEFVAL(0.0),
			DEFVAL(0), DEFVAL(Callable()), DEFVAL(Array()));
	ClassDB::bind_method(
			D_METHOD(
					"tween_property", "object", "key", "initial_val",
					"final_val", "duration", "ease_name", "delay", "time_type",
					"on_completed_callback", "arguments"),
			&TimeService::tween_property, DEFVAL("ease_in_out"), DEFVAL(0.0),
			DEFVAL(0), DEFVAL(Callable()), DEFVAL(Array()));
	ClassDB::bind_method(
			D_METHOD("clear_tween", "tween_id", "triggers_completed"),
			&TimeService::clear_tween, DEFVAL(false));

	ClassDB::bind_method(
			D_METHOD(
					"set_timeout", "callback", "delay", "arguments",
					"time_type"),
			&TimeService::set_timeout, DEFVAL(Array()), DEFVAL(0));
	ClassDB::bind_method(
			D_METHOD("clear_timeout", "timeout_id", "triggers_timeout"),
			&TimeService::clear_timeout, DEFVAL(false));

	ClassDB::bind_method(
			D_METHOD(
					"set_interval", "callback", "period", "arguments",
					"time_type"),
			&TimeService::set_interval, DEFVAL(Array()), DEFVAL(0));
	ClassDB::bind_method(
			D_METHOD("clear_interval", "interval_id", "triggers_interval"),
			&TimeService::clear_interval, DEFVAL(false));

	ClassDB::bind_method(
			D_METHOD(
					"throttle", "callback", "interval", "invokes_at_end",
					"time_type"),
			&TimeService::throttle, DEFVAL(true), DEFVAL(0));
	ClassDB::bind_method(
			D_METHOD("clear_throttle", "throttled_callback"),
			&TimeService::clear_throttle);

	ClassDB::bind_method(
			D_METHOD(
					"debounce", "callback", "interval", "invokes_at_start",
					"time_type"),
			&TimeService::debounce, DEFVAL(false), DEFVAL(0));
	ClassDB::bind_method(
			D_METHOD("clear_debounce", "debounced_callback"),
			&TimeService::clear_debounce);

	ADD_PROPERTY(
			PropertyInfo(Variant::FLOAT, "time_scale"), "set_time_scale",
			"get_time_scale");
	ADD_PROPERTY(
			PropertyInfo(Variant::FLOAT, "additional_debug_time_scale"),
			"set_additional_debug_time_scale",
			"get_additional_debug_time_scale");

	BIND_CONSTANT(physics_fps);
	BIND_CONSTANT(physics_time_step);

	// Bind TimeType values.
	BIND_ENUM_CONSTANT(APP_PHYSICS);
	BIND_ENUM_CONSTANT(APP_CLOCK);
	BIND_ENUM_CONSTANT(APP_PHYSICS_SCALED);
	BIND_ENUM_CONSTANT(APP_CLOCK_SCALED);
	BIND_ENUM_CONSTANT(APP_PHYSICS_FRAME_COUNT);
	BIND_ENUM_CONSTANT(APP_RENDER_FRAME_COUNT);
	BIND_ENUM_CONSTANT(PLAY_PHYSICS);
	BIND_ENUM_CONSTANT(PLAY_RENDER);
	BIND_ENUM_CONSTANT(PLAY_PHYSICS_SCALED);
	BIND_ENUM_CONSTANT(PLAY_RENDER_SCALED);
	BIND_ENUM_CONSTANT(PLAY_PHYSICS_FRAME_COUNT);
	BIND_ENUM_CONSTANT(PLAY_RENDER_FRAME_COUNT);

	// Bind ElapsedTimeType values.
	BIND_ENUM_CONSTANT(ELAPSED_PHYSICS_TIME);
	BIND_ENUM_CONSTANT(ELAPSED_PHYSICS_SCALED_TIME);
	BIND_ENUM_CONSTANT(ELAPSED_CLOCK_TIME);
	BIND_ENUM_CONSTANT(ELAPSED_CLOCK_SCALED_TIME);
	BIND_ENUM_CONSTANT(ELAPSED_RENDER_TIME);
	BIND_ENUM_CONSTANT(ELAPSED_RENDER_SCALED_TIME);
	BIND_ENUM_CONSTANT(PHYSICS_FRAME_COUNT);
	BIND_ENUM_CONSTANT(RENDER_FRAME_COUNT);
}
