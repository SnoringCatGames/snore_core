#include "snore_core/time/controller.h"

#include "snore_core/time/time_debouncer.h"
#include "snore_core/time/time_throttler.h"
#include "snore_core/time/time_type.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

const float TimeService::PHYSICS_FPS = 60.0;
const float TimeService::PHYSICS_TIME_STEP = 1.0 / PHYSICS_FPS;

const float TimeService::DEFAULT_TIME_SCALE = 1.0;
const float TimeService::DEFAULT_ADDITIONAL_DEBUG_TIME_SCALE = 1.0;
const float TimeService::GARBAGE_COLLECTION_INTERVAL = 30.0;

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

void TimeService::set_up() {}

void TimeService::reset() {}

TimeService::TimeService() {
	time_scale = DEFAULT_TIME_SCALE;
	additional_debug_time_scale = DEFAULT_ADDITIONAL_DEBUG_TIME_SCALE;
	_app_time = nullptr;
	_play_time = nullptr;
	_last_timeout_id = -1;
}

TimeService::~TimeService() {
	// Destructor implementation.
}

void TimeService::_init() { set_process_mode(PROCESS_MODE_ALWAYS); }

void TimeService::_ready() {
	_app_time = memnew(TimeTracker);
	_app_time->set_process_mode(PROCESS_MODE_ALWAYS);
	add_child(_app_time);

	_play_time = memnew(TimeTracker);
	_play_time->set_process_mode(PROCESS_MODE_PAUSABLE);
	add_child(_play_time);

	set_interval(
			callable_mp(this, &TimeService::collect_garbage),
			GARBAGE_COLLECTION_INTERVAL);
}

void TimeService::_process(double p_delta) {
	_handle_tweens();
	_handle_timeouts();
	_handle_intervals();
}

void TimeService::_handle_tweens() {
	Array finished_tween_ids;
	for (int i = 0; i < _tweens.size(); ++i) {
		Variant key = _tweens.get_key_list()[i];
		// TODO: Implement ScaffolderTween and proper handling.
		// ScaffolderTween *tween =
		// Object::cast_to<ScaffolderTween>(_tweens[key]); if (tween) {
		//     tween->step();
		//     if (!tween->is_active()) {
		//         finished_tween_ids.push_back(key);
		//     }
		// }
	}

	for (int i = 0; i < finished_tween_ids.size(); ++i) {
		_tweens.erase(finished_tween_ids[i]);
	}
}

void TimeService::_handle_timeouts() {
	int expired_timeout_id = -1;
	for (int i = 0; i < _timeouts.size(); ++i) {
		Variant key = _timeouts.get_key_list()[i];
		TimeTimeout *timeout = Object::cast_to<TimeTimeout>(_timeouts[key]);
		if (timeout && timeout->get_has_expired()) {
			expired_timeout_id = key;
			break;
		}
	}

	if (expired_timeout_id >= 0) {
		TimeTimeout *timeout =
				Object::cast_to<TimeTimeout>(_timeouts[expired_timeout_id]);
		if (timeout) {
			timeout->trigger();
		}
		_timeouts.erase(expired_timeout_id);
	}
}

void TimeService::_handle_intervals() {
	int triggered_interval_id = -1;
	for (int i = 0; i < _intervals.size(); ++i) {
		Variant key = _intervals.get_key_list()[i];
		TimeInterval *interval = Object::cast_to<TimeInterval>(_intervals[key]);
		if (interval && interval->get_has_reached_next_trigger_time()) {
			triggered_interval_id = key;
			break;
		}
	}

	if (triggered_interval_id >= 0) {
		TimeInterval *interval = Object::cast_to<TimeInterval>(
				_intervals[triggered_interval_id]);
		if (interval) {
			interval->trigger();
		}
	}
}

void TimeService::collect_garbage() {
	Array collections = Array::make(
			_timeouts, _intervals, _tweens, _throttled_callbacks,
			_debounced_callbacks);

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
			TimeTimeout *timeout = Object::cast_to<TimeTimeout>(obj);
			TimeInterval *interval = Object::cast_to<TimeInterval>(obj);

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
	_last_timeout_id += 1;
	return _last_timeout_id;
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
	return _play_time ? _play_time->get_physics_frame_count() : 0;
}

float TimeService::get_elapsed_time(int p_time_type) const {
	TimeTracker *tracker = _get_time_tracker_for_time_type(p_time_type);
	StringName key = _get_elapsed_time_key_for_time_type(p_time_type);

	if (!tracker) {
		return 0.0;
	}

	if (key == "elapsed_physics_time") {
		return tracker->get_elapsed_physics_time();
	} else if (key == "elapsed_physics_scaled_time") {
		return tracker->get_elapsed_physics_scaled_time();
	} else if (key == "elapsed_clock_time") {
		return tracker->get_elapsed_clock_time();
	} else if (key == "elapsed_clock_scaled_time") {
		return tracker->get_elapsed_clock_scaled_time();
	} else if (key == "elapsed_render_time") {
		return tracker->get_elapsed_render_time();
	} else if (key == "elapsed_render_scaled_time") {
		return tracker->get_elapsed_render_scaled_time();
	} else if (key == "physics_frame_count") {
		return static_cast<float>(tracker->get_physics_frame_count());
	} else if (key == "render_frame_count") {
		return static_cast<float>(tracker->get_render_frame_count());
	}

	return 0.0;
}

TimeTracker *TimeService::_get_time_tracker_for_time_type(
		int p_time_type) const {
	switch (p_time_type) {
		case TimeType::APP_PHYSICS:
		case TimeType::APP_CLOCK:
		case TimeType::APP_PHYSICS_SCALED:
		case TimeType::APP_CLOCK_SCALED:
		case TimeType::APP_PHYSICS_FRAME_COUNT:
		case TimeType::APP_RENDER_FRAME_COUNT:
			return _app_time;
		case TimeType::PLAY_PHYSICS:
		case TimeType::PLAY_RENDER:
		case TimeType::PLAY_PHYSICS_SCALED:
		case TimeType::PLAY_RENDER_SCALED:
		case TimeType::PLAY_PHYSICS_FRAME_COUNT:
		case TimeType::PLAY_RENDER_FRAME_COUNT:
			return _play_time;
		default:
			// TODO: Implement access to S.log.error().
			// S.log.error("Unrecognized time_type: %d", p_time_type);
			return nullptr;
	}
}

StringName TimeService::_get_elapsed_time_key_for_time_type(
		int p_time_type) const {
	switch (p_time_type) {
		case TimeType::APP_PHYSICS:
		case TimeType::PLAY_PHYSICS:
			return "elapsed_physics_time";
		case TimeType::APP_PHYSICS_SCALED:
		case TimeType::PLAY_PHYSICS_SCALED:
			return "elapsed_physics_scaled_time";
		case TimeType::APP_CLOCK:
			return "elapsed_clock_time";
		case TimeType::APP_CLOCK_SCALED:
			return "elapsed_clock_scaled_time";
		case TimeType::PLAY_RENDER:
			return "elapsed_render_time";
		case TimeType::PLAY_RENDER_SCALED:
			return "elapsed_render_scaled_time";
		case TimeType::APP_PHYSICS_FRAME_COUNT:
		case TimeType::PLAY_PHYSICS_FRAME_COUNT:
			return "physics_frame_count";
		case TimeType::APP_RENDER_FRAME_COUNT:
		case TimeType::PLAY_RENDER_FRAME_COUNT:
			return "render_frame_count";
		default:
			// TODO: Implement access to S.log.error().
			// S.log.error("Unrecognized time_type: %d", p_time_type);
			return "";
	}
}

int TimeService::_get_time_type_from_key(
		const StringName &p_elapsed_time_key) const {
	if (p_elapsed_time_key == "elapsed_physics_time") {
		return TimeType::APP_PHYSICS; // Default to APP_PHYSICS
	} else if (p_elapsed_time_key == "elapsed_physics_scaled_time") {
		return TimeType::APP_PHYSICS_SCALED;
	} else if (p_elapsed_time_key == "elapsed_clock_time") {
		return TimeType::APP_CLOCK;
	} else if (p_elapsed_time_key == "elapsed_clock_scaled_time") {
		return TimeType::APP_CLOCK_SCALED;
	} else if (p_elapsed_time_key == "elapsed_render_time") {
		return TimeType::PLAY_RENDER;
	} else if (p_elapsed_time_key == "elapsed_render_scaled_time") {
		return TimeType::PLAY_RENDER_SCALED;
	} else if (p_elapsed_time_key == "physics_frame_count") {
		return TimeType::APP_PHYSICS_FRAME_COUNT;
	} else if (p_elapsed_time_key == "render_frame_count") {
		return TimeType::APP_RENDER_FRAME_COUNT;
	} else {
		// Default to APP_PHYSICS
		return TimeType::APP_PHYSICS;
	}
}

float TimeService::get_combined_scale() const {
	return time_scale * additional_debug_time_scale;
}

float TimeService::scale_delta(float p_duration) const {
	return p_duration * get_combined_scale();
}

float TimeService::get_scaled_time_step() const {
	return PHYSICS_TIME_STEP * get_combined_scale();
}

void TimeService::_set_time_scale(float p_value) {
	time_scale = p_value;
	if (_app_time) {
		_app_time->set_time_scale(get_combined_scale());
	}
	if (_play_time) {
		_play_time->set_time_scale(get_combined_scale());
	}
}

void TimeService::_set_additional_debug_time_scale(float p_value) {
	additional_debug_time_scale = p_value;
	if (_app_time) {
		_app_time->set_time_scale(get_combined_scale());
	}
	if (_play_time) {
		_play_time->set_time_scale(get_combined_scale());
	}
}

int TimeService::tween_method(
		Object *p_object,
		const StringName &p_key,
		const Variant &p_initial_val,
		const Variant &p_final_val,
		float p_duration,
		const StringName &p_ease_name,
		float p_delay,
		int p_time_type,
		const Callable &p_on_completed_callback,
		const Array &p_arguments) {
	return _tween(
			p_object, p_key, false, p_initial_val, p_final_val, p_duration,
			p_ease_name, p_delay, p_time_type, p_on_completed_callback,
			p_arguments);
}

int TimeService::tween_property(
		Object *p_object,
		const StringName &p_key,
		const Variant &p_initial_val,
		const Variant &p_final_val,
		float p_duration,
		const StringName &p_ease_name,
		float p_delay,
		int p_time_type,
		const Callable &p_on_completed_callback,
		const Array &p_arguments) {
	return _tween(
			p_object, p_key, true, p_initial_val, p_final_val, p_duration,
			p_ease_name, p_delay, p_time_type, p_on_completed_callback,
			p_arguments);
}

int TimeService::_tween(
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
		const Array &p_arguments) {
	// TODO: Implement ScaffolderTween creation and management.
	// ScaffolderTween *tween = memnew(ScaffolderTween(p_object, false));
	// tween->_interpolate(p_object, p_key, p_is_property, p_initial_val,
	// p_final_val, p_duration, p_ease_name, p_delay, p_time_type); if
	// (p_on_completed_callback.is_valid()) {
	//     tween->connect("tween_all_completed", callable_mp(this,
	//     &TimeService::_call_tween_completed_callback).bind(p_on_completed_callback,
	//     p_arguments));
	// }
	// tween->start();
	// _tweens[tween->get_id()] = tween;
	// return tween->get_id();
	return -1; // Placeholder
}

void TimeService::_call_tween_completed_callback(
		const Callable &p_on_completed_callback,
		const Array &p_arguments) {
	p_on_completed_callback.callv(p_arguments);
}

bool TimeService::clear_tween(int p_tween_id, bool p_triggers_completed) {
	if (!_tweens.has(p_tween_id)) {
		return false;
	}
	// TODO: Implement proper tween handling.
	// ScaffolderTween *tween =
	// Object::cast_to<ScaffolderTween>(_tweens[p_tween_id]); if
	// (p_triggers_completed && tween) {
	//     tween->trigger_completed();
	// }
	// if (tween) {
	//     tween->free();
	// }
	_tweens.erase(p_tween_id);
	return true;
}

int TimeService::set_timeout(
		const Callable &p_callback,
		float p_delay,
		const Array &p_arguments,
		int p_time_type) {
	TimeTimeout *timeout = memnew(TimeTimeout);
	timeout->initialize(
			this, p_callback.get_object(), p_time_type, p_callback, p_delay,
			p_arguments);
	_timeouts[timeout->get_id()] = timeout;
	return timeout->get_id();
}

bool TimeService::clear_timeout(int p_timeout_id, bool p_triggers_timeout) {
	if (!_timeouts.has(p_timeout_id)) {
		return false;
	}
	if (p_triggers_timeout) {
		TimeTimeout *timeout =
				Object::cast_to<TimeTimeout>(_timeouts[p_timeout_id]);
		if (timeout) {
			timeout->trigger();
		}
	}
	_timeouts.erase(p_timeout_id);
	return true;
}

int TimeService::set_interval(
		const Callable &p_callback,
		float p_period,
		const Array &p_arguments,
		int p_time_type) {
	TimeInterval *interval = memnew(TimeInterval);
	interval->initialize(
			this, p_callback.get_object(), p_time_type, p_callback, p_period,
			p_arguments);
	_intervals[interval->get_id()] = interval;
	return interval->get_id();
}

bool TimeService::clear_interval(int p_interval_id, bool p_triggers_interval) {
	if (!_intervals.has(p_interval_id)) {
		return false;
	}
	if (p_triggers_interval) {
		TimeInterval *interval =
				Object::cast_to<TimeInterval>(_intervals[p_interval_id]);
		if (interval) {
			interval->trigger();
		}
	}
	_intervals.erase(p_interval_id);
	return true;
}

Callable TimeService::throttle(
		const Callable &p_callback,
		float p_interval,
		bool p_invokes_at_end,
		int p_time_type) {
	TimeThrottler *throttler = memnew(TimeThrottler);
	throttler->initialize(
			p_callback.get_object(), this, p_time_type, p_callback, p_interval,
			p_invokes_at_end);
	_throttled_callbacks[throttler->get_on_call()] = throttler;
	return throttler->get_on_call();
}

bool TimeService::clear_throttle(const Callable &p_throttled_callback) {
	if (!_throttled_callbacks.has(p_throttled_callback)) {
		return false;
	}
	TimeThrottler *throttler = Object::cast_to<TimeThrottler>(
			_throttled_callbacks[p_throttled_callback]);
	if (throttler) {
		throttler->cancel();
	}
	_throttled_callbacks.erase(p_throttled_callback);
	return true;
}

Callable TimeService::debounce(
		const Callable &p_callback,
		float p_interval,
		bool p_invokes_at_start,
		int p_time_type) {
	TimeDebouncer *debouncer = memnew(TimeDebouncer);
	debouncer->initialize(
			p_callback.get_object(), this, p_time_type, p_callback, p_interval,
			p_invokes_at_start);
	_debounced_callbacks[debouncer->get_on_call()] = debouncer;
	return debouncer->get_on_call();
}

bool TimeService::clear_debounce(const Callable &p_debounced_callback) {
	if (!_debounced_callbacks.has(p_debounced_callback)) {
		return false;
	}
	TimeDebouncer *debouncer = Object::cast_to<TimeDebouncer>(
			_debounced_callbacks[p_debounced_callback]);
	if (debouncer) {
		debouncer->cancel();
	}
	_debounced_callbacks.erase(p_debounced_callback);
	return true;
}

void TimeService::_bind_methods() {
	ClassDB::bind_method(
			D_METHOD("collect_garbage"), &TimeService::collect_garbage);
	ClassDB::bind_method(
			D_METHOD("get_next_task_id"), &TimeService::get_next_task_id);

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
			&TimeService::_set_time_scale);

	ClassDB::bind_method(
			D_METHOD("get_additional_debug_time_scale"),
			&TimeService::get_additional_debug_time_scale);
	ClassDB::bind_method(
			D_METHOD(
					"set_additional_debug_time_scale",
					"additional_debug_time_scale"),
			&TimeService::_set_additional_debug_time_scale);

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

	BIND_CONSTANT(PHYSICS_FPS);
	BIND_CONSTANT(PHYSICS_TIME_STEP);
}
