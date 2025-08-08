#ifndef TWEEN_H
#define TWEEN_H

#include "snore_core/time/ease_type.h"
#include "snore_core/time/time_type.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>

#include <vector>

namespace godot {

class Callable;
class NodePath;
class StringName;
class SubTween;
class Variant;

// A tween system that supports tracking many different modes of time.
class SnoreCoreTween : public Node {
	GDCLASS(SnoreCoreTween, Node)

public:
	SnoreCoreTween() = default;
	~SnoreCoreTween();

	void _init(Node *p_parent, bool p_adds_self_as_child_of_parent = true);
	void _destroy();

	bool is_active() const;
	float get_progress() const;

	void step();

	bool start();
	bool stop(Object *p_object, const StringName &p_key = "");
	bool stop_all();
	void trigger_completed();

	void interpolate_method(
			Object *p_object,
			const StringName &p_key,
			const Variant &p_initial_val,
			const Variant &p_final_val,
			float p_duration,
			EaseType p_ease_type = EaseType::EASE_IN_OUT,
			float p_delay = 0.0,
			TimeType p_time_type = TimeType::APP_PHYSICS);

	void interpolate_property(
			Object *p_object,
			const NodePath &p_key,
			const Variant &p_initial_val,
			const Variant &p_final_val,
			float p_duration,
			EaseType p_ease_type = EaseType::EASE_IN_OUT,
			float p_delay = 0.0,
			TimeType p_time_type = TimeType::APP_PHYSICS);

	Node *get_parent_node() const { return parent; }
	int get_id() const { return id; }

protected:
	static void _bind_methods() {}

private:
	Node *parent = nullptr;
	int id = -1;

	std::vector<Ref<SubTween>> pending_sub_tweens;
	std::vector<Ref<SubTween>> active_sub_tweens;

	void interpolate(
			Object *p_object,
			const Variant &p_key,
			bool p_is_property,
			const Variant &p_initial_val,
			const Variant &p_final_val,
			float p_duration,
			EaseType p_ease_type,
			float p_delay,
			TimeType p_time_type);

	void stop_sub_tween(Ref<SubTween> p_sub_tween);
};

// Internal sub-tween class equivalent to _SubTween in GDScript.
class SubTween : public RefCounted {
	GDCLASS(SubTween, RefCounted)

public:
	SubTween() = default;
	~SubTween() = default;

	void initialize(
			Object *p_object,
			const Variant &p_key,
			bool p_is_property,
			const Variant &p_initial_val,
			const Variant &p_final_val,
			float p_duration,
			EaseType p_ease_type,
			float p_delay,
			TimeType p_time_type);

	bool get_is_finished() const;
	void start();
	void end();
	void step();

	Object *get_object() const { return object; }
	StringName get_key() const { return key; }
	float get_progress() const { return progress; }

protected:
	static void _bind_methods() {}

private:
	Object *object = nullptr;
	StringName key;
	bool is_property = false;
	Variant initial_val;
	Variant final_val;
	float duration = 0.0;
	EaseType ease_type = EaseType::EASE_IN_OUT;
	float delay = 0.0;
	TimeType time_type = TimeType::APP_PHYSICS;

	float start_time = INFINITY;
	float progress = 0.0;

	void update_with_value(const Variant &p_value);
};

} // namespace godot

#endif // TWEEN_H
