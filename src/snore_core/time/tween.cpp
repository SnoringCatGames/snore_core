#include "snore_core/time/tween.h"

#include "snore_core/internal/registration_utils.h"
#include "snore_core/snore_core_utils.h"
#include "snore_core/time/time_service.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

SnoreCoreTween::~SnoreCoreTween() {
	// Clean up any remaining sub-tweens.
	for (Ref<SubTween> sub_tween : pending_sub_tweens) {
		if (is_valid(sub_tween)) {
			sub_tween->unreference();
		}
	}
	for (Ref<SubTween> sub_tween : active_sub_tweens) {
		if (is_valid(sub_tween)) {
			sub_tween->unreference();
		}
	}
	pending_sub_tweens.clear();
	active_sub_tweens.clear();
}

void SnoreCoreTween::_init(
		Node *p_parent,
		bool p_adds_self_as_child_of_parent) {
	if (!ENSURE(p_parent, "Parent node cannot be null.") ||
		!ENSURE(p_parent->is_inside_tree(),
				"Parent node must be inside the scene tree.")) {
		return;
	}

	set_process(true);

	parent = p_parent;
	set_name("Tween");
	id = TimeService::get()->get_next_task_id();
	if (p_adds_self_as_child_of_parent) {
		parent->add_child(this);
	}
}

void SnoreCoreTween::_destroy() {
	if (TimeService *time_service = TimeService::get_maybe()) {
		time_service->clear_tween(id);
	}
}

void SnoreCoreTween::step() {
	if (!is_active()) {
		return;
	}

	// End and remove any finished sub-tweens.
	std::vector<Ref<SubTween>> finished_sub_tweens;
	for (Ref<SubTween> sub_tween : active_sub_tweens) {
		if (sub_tween->get_is_finished()) {
			finished_sub_tweens.push_back(sub_tween);
		}
	}
	for (Ref<SubTween> sub_tween : finished_sub_tweens) {
		sub_tween->end();
		stop_sub_tween(sub_tween);
		emit_signal(
				"_tween_completed", sub_tween->get_object(),
				sub_tween->get_key());
	}

	// Update all in-progress tweens.
	for (Ref<SubTween> sub_tween : active_sub_tweens) {
		sub_tween->step();
	}

	if (!is_active()) {
		emit_signal("tween_all_completed");
	}
}

bool SnoreCoreTween::is_active() const { return !active_sub_tweens.empty(); }

float SnoreCoreTween::get_progress() const {
	if (active_sub_tweens.empty()) {
		return 0.0;
	}
	return is_valid(active_sub_tweens[0]) ? active_sub_tweens[0]->get_progress()
										  : 0.0;
}

bool SnoreCoreTween::start() {
	if (pending_sub_tweens.empty()) {
		return false;
	}

	for (Ref<SubTween> sub_tween : pending_sub_tweens) {
		sub_tween->start();
		active_sub_tweens.push_back(sub_tween);
	}
	pending_sub_tweens.clear();

	return true;
}

bool SnoreCoreTween::stop(Object *p_object, const StringName &p_key) {
	for (std::vector<Ref<SubTween>>::iterator it = active_sub_tweens.begin();
		 it != active_sub_tweens.end(); ++it) {
		Ref<SubTween> sub_tween = *it;
		if (sub_tween->get_object() == p_object &&
			(p_key.is_empty() || sub_tween->get_key() == p_key)) {
			active_sub_tweens.erase(it);
			return true;
		}
	}
	return false;
}

void SnoreCoreTween::stop_sub_tween(Ref<SubTween> p_sub_tween) {
	std::vector<Ref<SubTween>>::iterator it = std::find(
			active_sub_tweens.begin(), active_sub_tweens.end(), p_sub_tween);
	if (it != active_sub_tweens.end()) {
		active_sub_tweens.erase(it);
	}
}

bool SnoreCoreTween::stop_all() {
	if (active_sub_tweens.empty()) {
		return false;
	}
	active_sub_tweens.clear();
	return true;
}

void SnoreCoreTween::trigger_completed() {
	if (active_sub_tweens.empty()) {
		return;
	}

	emit_signal("tween_all_completed");
}

void SnoreCoreTween::interpolate_method(
		Object *p_object,
		const StringName &p_key,
		const Variant &p_initial_val,
		const Variant &p_final_val,
		float p_duration,
		EaseType p_ease_type,
		float p_delay,
		TimeType p_time_type) {
	interpolate(
			p_object, p_key, false, p_initial_val, p_final_val, p_duration,
			p_ease_type, p_delay, p_time_type);
}

void SnoreCoreTween::interpolate_property(
		Object *p_object,
		const NodePath &p_key,
		const Variant &p_initial_val,
		const Variant &p_final_val,
		float p_duration,
		EaseType p_ease_type,
		float p_delay,
		TimeType p_time_type) {
	interpolate(
			p_object, p_key, true, p_initial_val, p_final_val, p_duration,
			p_ease_type, p_delay, p_time_type);
}

void SnoreCoreTween::interpolate(
		Object *p_object,
		const Variant &p_key,
		bool p_is_property,
		const Variant &p_initial_val,
		const Variant &p_final_val,
		float p_duration,
		EaseType p_ease_type,
		float p_delay,
		TimeType p_time_type) {
	Ref<SubTween> sub_tween = instantiate_ref<SubTween>();
	sub_tween->initialize(
			p_object, p_key, p_is_property, p_initial_val, p_final_val,
			p_duration, p_ease_type, p_delay, p_time_type);
	pending_sub_tweens.push_back(sub_tween);
}

void SubTween::initialize(
		Object *p_object,
		const Variant &p_key,
		bool p_is_property,
		const Variant &p_initial_val,
		const Variant &p_final_val,
		float p_duration,
		EaseType p_ease_type,
		float p_delay,
		TimeType p_time_type) {
	ENSURE_SIMPLE(p_duration > 0.0);

	object = p_object;
	key = p_key.operator StringName();
	is_property = p_is_property;
	initial_val = p_initial_val;
	final_val = p_final_val;
	duration = p_duration;
	ease_type = p_ease_type;
	delay = p_delay;
	time_type = p_time_type;
}

bool SubTween::get_is_finished() const {
	const TimeService *time_service = TimeService::get_maybe();
	if (!time_service) {
		return true;
	}

	const float current_time = time_service->get_elapsed_time(time_type);
	return (current_time >= start_time + duration + delay) || !is_valid(object);
}

void SubTween::start() {
	const TimeService *time_service = TimeService::get_maybe();
	if (time_service) {
		start_time = time_service->get_elapsed_time(time_type);
	}
}

void SubTween::end() {
	if (is_valid(object)) {
		update_with_value(final_val);
	}
}

void SubTween::step() {
	const TimeService *time_service = TimeService::get_maybe();
	if (!time_service) {
		return;
	}

	const float current_time = time_service->get_elapsed_time(time_type);
	const float elapsed_time = current_time - start_time;

	if (elapsed_time < delay) {
		return;
	}

	progress = Math::clamp((elapsed_time - delay) / duration, 0.0f, 1.0f);
	progress = SnoreCoreUtils::ease(progress, ease_type);

	const Variant lerped_value = lerp(initial_val, final_val, progress);

	update_with_value(lerped_value);
}

void SubTween::update_with_value(const Variant &p_value) {
	if (!is_valid(object)) {
		return;
	}

	if (is_property) {
		object->set_indexed(NodePath(key), p_value);
	} else {
		Array args;
		args.push_back(p_value);
		object->callv(key, args);
	}
}
