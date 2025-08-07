#ifndef SNORE_CORE_UTILS_H
#define SNORE_CORE_UTILS_H

#include "snore_core/snore_core_main_module.h"
#include "snore_core/snore_core_submodule.h"

namespace godot {

class Array;
class Color;
class Dictionary;
template <typename T> class TypedArray;
class PackedVector2Array;
class Vector2;
class Node;
class String;
class StringName;
class Variant;
class Control;
enum Control::MouseFilter;
class ScrollContainer;
class Object;
class SceneState;

class SnoreCoreUtils : public SnoreCoreSubmodule {
	GDCLASS(SnoreCoreUtils, SnoreCoreSubmodule)
	SC_SUBMODULE_CLASS(SnoreCoreUtils, SnoreCore)

public:
	static const constexpr int64_t MAX_INT = 9223372036854775807;

	SnoreCoreUtils() = default;
	virtual ~SnoreCoreUtils() = default;

	static bool ensure(bool p_condition, const String &p_message = "");

	static void splice(
			Array &p_result,
			int p_start,
			int p_delete_count,
			const Array &p_items_to_insert);

	static Array dedup(const Array &p_array);

	static void subtract_nested_arrays(
			Dictionary *p_result,
			const Dictionary *p_other,
			bool p_expects_no_missing_matches = false);

	static void subtract_arrays(
			Array *p_result,
			const Array *p_other,
			bool p_expects_no_missing_matches = false);

	static Dictionary array_to_set(const Array &p_array);

	static Array cascade_sort(Array p_arr);

	static PackedVector2Array translate_polyline(
			const PackedVector2Array &p_vertices,
			const Vector2 &p_translation);

	static void clear_children(Node *p_node);

	static float ease(float p_progress, EaseType p_ease_type);

	static bool is_num(const Variant &p_v);
	static Vector2 floor_vector(const Vector2 &p_v);
	static Vector2 ceil_vector(const Vector2 &p_v);
	static Vector2 round_vector(const Vector2 &p_v);

	static Variant mix(
			const Array &p_values,
			const TypedArray<float> &p_weights);
	static Color mix_colors(
			const Array &p_colors,
			const TypedArray<float> &p_weights);

	static String get_datetime_string();
	static String get_time_string();
	static String get_time_string_from_seconds(
			float p_time,
			bool p_includes_ms = false,
			bool p_includes_empty_hours = true,
			bool p_includes_empty_minutes = true);

	static String get_vector_string(
			const Vector2 &p_vector,
			int p_decimal_place_count = 2);

	static String get_spaces(int p_count);
	static String pad_string(
			const String &p_string,
			int p_length,
			bool p_pads_on_right = true,
			bool p_allows_longer_strings = false);

	static String resize_string(
			const String &p_string,
			int p_length,
			bool p_pads_on_right = true);

	static void take_screenshot();
	static void open_screenshot_folder();

	static void set_mouse_filter_recursively(
			Node *p_node,
			Control::MouseFilter p_mouse_filter);

	static int get_node_vscroll_position(
			ScrollContainer *p_scroll_container,
			Control *p_control,
			int p_offset = 0);

	static int64_t get_instance_id_or_not(Object *p_object);

	static Array get_all_nodes_in_group(const StringName &p_group_name);
	static Node *get_node_in_group(const StringName &p_group_name);

	static Variant get_property_value_from_scene_state_node(
			SceneState *p_state,
			int p_node_index,
			const StringName &p_property_name,
			bool p_expects_a_result = false);

	static bool check_whether_sub_classes_are_tools(Object *p_object);
	static bool is_running_in_isolated_scene_mode();

	static StringName get_type_string(int p_type);
	static String get_display_name(const Variant &p_object);

	static bool get_were_screenshots_taken() { return were_screenshots_taken; }

protected:
	void _bind_methods();

private:
	Control *focus_releaser = nullptr;
	static bool were_screenshots_taken;

	static float ease_type_to_param(EaseType p_type);
};

} //namespace godot

#endif // SNORE_CORE_UTILS_H
