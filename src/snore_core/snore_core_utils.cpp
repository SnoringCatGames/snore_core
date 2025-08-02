#include "snore_core/snore_core_utils.h"

#include "snore_core/geometry_constants.h"
#include "snore_core/internal/debug_utils.h"
#include "snore_core/logger.h"

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/canvas_item.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/reg_ex.hpp>
#include <godot_cpp/classes/reg_ex_match.hpp>
#include <godot_cpp/classes/resource_uid.hpp>
#include <godot_cpp/classes/scene_state.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/classes/scroll_container.hpp>
#include <godot_cpp/classes/slider.hpp>
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/classes/v_scroll_bar.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/vector2.hpp>

#include <unordered_set>

using namespace godot;

void SnoreCoreUtils::set_up() {
	focus_releaser = memnew(Button);
	focus_releaser->set_modulate(Color(1.0, 1.0, 1.0, 0.0));
	focus_releaser->set_visible(false);
	SnoreCore::get()->add_node_to_root(focus_releaser, "FocusReleaser");
}

void SnoreCoreUtils::reset() {
	focus_releaser->queue_free();
	focus_releaser = nullptr;
}

bool SnoreCoreUtils::ensure(bool p_condition, const String &p_message) {
	if (!ENSURE(p_condition, p_message)) {
		return false;
	}
	return true;
}

void SnoreCoreUtils::splice(
		Array &p_result,
		int p_start,
		int p_delete_count,
		const Array &p_items_to_insert) {
	const int old_count = p_result.size();
	const int items_to_insert_count = p_items_to_insert.size();

	ENSURE(p_start >= 0, "Start index must be non-negative");
	ENSURE(p_start <= old_count, "Start index must be within array bounds");
	ENSURE(p_delete_count >= 0, "Delete count must be non-negative");
	ENSURE(p_delete_count <= old_count,
		   "Delete count must not exceed array size");
	ENSURE(p_start + p_delete_count <= old_count,
		   "Start + delete_count must not exceed array size");

	const int new_count = old_count - p_delete_count + items_to_insert_count;
	const bool is_growing = items_to_insert_count > p_delete_count;
	const bool is_shrinking = items_to_insert_count < p_delete_count;
	const int displacement = items_to_insert_count - p_delete_count;

	if (is_shrinking) {
		// Shift old items toward the front.
		for (int i = p_start + p_delete_count; i < old_count; i++) {
			p_result[i + displacement] = p_result[i];
		}
	}

	// Resize the result array.
	p_result.resize(new_count);

	if (is_growing) {
		// Shift old items toward the back.
		for (int i = old_count - 1; i >= p_start + p_delete_count; i--) {
			p_result[i + displacement] = p_result[i];
		}
	}

	// Insert the new items.
	for (int i = 0; i < items_to_insert_count; i++) {
		p_result[p_start + i] = p_items_to_insert[i];
	}
}

Array SnoreCoreUtils::dedup(const Array &p_array) {
	std::unordered_set<Variant> set;
	for (int i = 0; i < p_array.size(); i++) {
		set.insert(p_array[i]);
	}
	const int deduped_size = set.size();
	set.clear();
	Array result;
	result.resize(deduped_size);
	for (int i = 0; i < p_array.size(); i++) {
		if (set.find(p_array[i]) == set.end()) {
			result[i++] = p_array[i];
			set.insert(p_array[i]);
		}
	}
	return result;
}

void SnoreCoreUtils::subtract_nested_arrays(
		Dictionary *p_result,
		const Dictionary *p_other,
		bool p_expects_no_missing_matches) {
	CHECK_SIMPLE(p_result != nullptr);
	CHECK_SIMPLE(p_other != nullptr);
	Array keys = p_other->keys();
	for (int i = 0; i < keys.size(); i++) {
		const Variant &key = keys[i];
		if (p_result->has(key)) {
			Variant &result_value = (*p_result)[key];
			const Variant &other_value = (*p_other)[key];
			if (result_value.get_type() == Variant::DICTIONARY &&
				other_value.get_type() == Variant::DICTIONARY) {
				Dictionary *result_dict =
						Object::cast_to<Dictionary>(result_value);
				const Dictionary *other_dict =
						Object::cast_to<Dictionary>(other_value);
				subtract_nested_arrays(
						result_dict, other_dict, p_expects_no_missing_matches);
			} else if (
					result_value.get_type() == Variant::ARRAY &&
					other_value.get_type() == Variant::ARRAY) {
				Array *result_array = Object::cast_to<Array>(result_value);
				const Array *other_array = Object::cast_to<Array>(other_value);
				subtract_arrays(
						result_array, other_array,
						p_expects_no_missing_matches);
			} else if (!ENSURE(!p_expects_no_missing_matches,
							   vformat("Wrong-type match: "
									   "(We currently don't support "
									   "subtracting properties "
									   "from a Dictionary. We only support "
									   "subtracting "
									   "elements from Arrays.)"
									   "\n    key=%s,\n    result=%s,\n    "
									   "other=%s",
									   key, p_result, p_other))) {
				return;
			}
		} else if (!ENSURE(!p_expects_no_missing_matches,
						   vformat("Missing match: "
								   "\n    key=%s,\n    result=%s,\n    "
								   "other=%s",
								   key, p_result, p_other))) {
			return;
		}
	}
	return;
}

void SnoreCoreUtils::subtract_arrays(
		Array *p_result,
		const Array *p_other,
		bool p_expects_no_missing_matches) {
	for (int i = 0; i < p_other->size(); i++) {
		const Variant &element = (*p_other)[i];
		const int result_index = p_result->find(element);
		if (result_index >= 0) {
			p_result->remove_at(result_index);
		} else if (!ENSURE(!p_expects_no_missing_matches,
						   vformat("Missing match: "
								   "\n    element=%s,\n    result=%s,\n    "
								   "other=%s",
								   element, p_result, p_other))) {
			return;
		}
	}
	return;
}

Dictionary SnoreCoreUtils::array_to_set(const Array &p_array) {
	Dictionary set;
	for (int i = 0; i < p_array.size(); i++) {
		set[p_array[i]] = true;
	}
	return set;
}

Array SnoreCoreUtils::cascade_sort(Array p_arr) {
	p_arr.sort();
	return p_arr;
}

PackedVector2Array SnoreCoreUtils::translate_polyline(
		const PackedVector2Array &p_vertices,
		const Vector2 &p_translation) {
	PackedVector2Array result;
	result.resize(p_vertices.size());
	for (int i = 0; i < p_vertices.size(); i++) {
		result[i] = p_vertices[i] + p_translation;
	}
	return result;
}

void SnoreCoreUtils::clear_children(Node *p_node) {
	Array children = p_node->get_children();
	for (int i = 0; i < children.size(); i++) {
		Node *child = Object::cast_to<Node>(children[i]);
		if (child) {
			child->queue_free();
		}
	}
}

float SnoreCoreUtils::ease_name_to_param(const StringName &p_name) {
	if (p_name == StringName("linear")) {
		return 1.0;
	} else if (p_name == StringName("ease_in")) {
		return 2.4;
	} else if (p_name == StringName("ease_in_strong")) {
		return 4.8;
	} else if (p_name == StringName("ease_in_very_strong")) {
		return 9.6;
	} else if (p_name == StringName("ease_in_weak")) {
		return 1.6;
	} else if (p_name == StringName("ease_out")) {
		return 0.4;
	} else if (p_name == StringName("ease_out_strong")) {
		return 0.2;
	} else if (p_name == StringName("ease_out_very_strong")) {
		return 0.1;
	} else if (p_name == StringName("ease_out_weak")) {
		return 0.6;
	} else if (p_name == StringName("ease_in_out")) {
		return -2.4;
	} else if (p_name == StringName("ease_in_out_strong")) {
		return -4.8;
	} else if (p_name == StringName("ease_in_out_very_strong")) {
		return -9.6;
	} else if (p_name == StringName("ease_in_out_weak")) {
		return -1.8;
	} else {
		ENSURE(false, "Unknown ease name: " + p_name);
		return infinity;
	}
}

// TODO: This was copied from Godot's Math::ease, because godot-cpp does not
//       expose the `ease` API. If this gets added to godot-cpp, use that and
//       remove this function.
double ease(double p_x, double p_c) {
	if (p_x < 0) {
		p_x = 0;
	} else if (p_x > 1.0) {
		p_x = 1.0;
	}
	if (p_c > 0) {
		if (p_c < 1.0) {
			return 1.0 - Math::pow(1.0 - p_x, 1.0 / p_c);
		} else {
			return Math::pow(p_x, p_c);
		}
	} else if (p_c < 0) {
		//inout ease
		if (p_x < 0.5) {
			return Math::pow(p_x * 2.0, -p_c) * 0.5;
		} else {
			return (1.0 - Math::pow(1.0 - (p_x - 0.5) * 2.0, -p_c)) * 0.5 + 0.5;
		}
	} else {
		// no ease (raw)
		return 0;
	}
}

float SnoreCoreUtils::ease_by_name(
		float p_progress,
		const StringName &p_ease_name) {
	return ease(p_progress, ease_name_to_param(p_ease_name));
}

bool SnoreCoreUtils::is_num(const Variant &p_v) {
	return p_v.get_type() == Variant::INT || p_v.get_type() == Variant::FLOAT;
}

Vector2 SnoreCoreUtils::floor_vector(const Vector2 &p_v) {
	return Vector2(Math::floor(p_v.x), Math::floor(p_v.y));
}

Vector2 SnoreCoreUtils::ceil_vector(const Vector2 &p_v) {
	return Vector2(Math::ceil(p_v.x), Math::ceil(p_v.y));
}

Vector2 SnoreCoreUtils::round_vector(const Vector2 &p_v) {
	return Vector2(Math::round(p_v.x), Math::round(p_v.y));
}

Variant SnoreCoreUtils::mix(
		const Array &p_values,
		const TypedArray<float> &p_weights) {
	ENSURE(p_values.size() == p_weights.size(),
		   "Values and weights arrays must have same size");
	ENSURE(!p_values.is_empty(), "Values array cannot be empty");

	const int count = p_values.size();

	float weight_sum = 0.0;
	for (int i = 0; i < p_weights.size(); i++) {
		weight_sum += float(p_weights[i]);
	}

	Variant weighted_average;
	const Variant first_value = p_values[0];
	if (is_num(first_value)) {
		weighted_average = 0.0;
	} else if (first_value.get_type() == Variant::VECTOR2) {
		weighted_average = vector2_zero;
	} else if (first_value.get_type() == Variant::VECTOR3) {
		weighted_average = vector3_zero;
	} else {
		ENSURE(false, "Unsupported type: " + first_value.stringify());
		return Variant();
	}

	for (int i = 0; i < count; i++) {
		const Variant value = p_values[i];
		const float weight = p_weights[i];
		const float normalized_weight =
				weight_sum > 0.0 ? weight / weight_sum : 1.0 / count;

		if (is_num(value)) {
			weighted_average =
					float(weighted_average) + float(value) * normalized_weight;
		} else if (value.get_type() == Variant::VECTOR2) {
			weighted_average = Vector2(weighted_average) +
					Vector2(value) * normalized_weight;
		} else if (value.get_type() == Variant::VECTOR3) {
			weighted_average = Vector3(weighted_average) +
					Vector3(value) * normalized_weight;
		}
	}

	return weighted_average;
}

Color SnoreCoreUtils::mix_colors(
		const Array &p_colors,
		const TypedArray<float> &p_weights) {
	ENSURE(p_colors.size() == p_weights.size(),
		   "Colors and weights arrays must have same size");
	ENSURE(!p_colors.is_empty(), "Colors array cannot be empty");

	const int count = p_colors.size();

	float weight_sum = 0.0;
	for (int i = 0; i < p_weights.size(); i++) {
		weight_sum += float(p_weights[i]);
	}

	float h = 0.0;
	float s = 0.0;
	float v = 0.0;
	for (int i = 0; i < count; i++) {
		const Color color = p_colors[i];
		const float weight = p_weights[i];
		const float normalized_weight =
				weight_sum > 0.0 ? weight / weight_sum : 1.0 / count;
		h += color.get_h() * normalized_weight;
		s += color.get_s() * normalized_weight;
		v += color.get_v() * normalized_weight;
	}

	return Color::from_hsv(h, s, v, 1.0);
}

String SnoreCoreUtils::get_datetime_string() {
	const Dictionary datetime =
			Time::get_singleton()->get_datetime_dict_from_system();
	return vformat(
			"%s-%s-%s_%s:%s:%s.%s", datetime["year"], datetime["month"],
			datetime["day"], datetime["hour"], datetime["minute"],
			datetime["second"], datetime["millisecond"]);
}

String SnoreCoreUtils::get_time_string() {
	const Dictionary datetime =
			Time::get_singleton()->get_datetime_dict_from_system();
	return vformat(
			"%02d:%02d:%02d.%03d", datetime["hour"], datetime["minute"],
			datetime["second"], datetime["millisecond"]);
}

String SnoreCoreUtils::get_time_string_from_seconds(
		float p_time,
		bool p_includes_ms,
		bool p_includes_empty_hours,
		bool p_includes_empty_minutes) {
	const bool is_undefined = Math::is_inf(p_time);
	String time_str = "";

	// Hours.
	const int hours = int(p_time / 3600.0f);
	p_time = Math::fmod(p_time, 3600.0f);
	if (hours != 0 || p_includes_empty_hours) {
		if (!is_undefined) {
			time_str = vformat("%s%02d:", time_str, hours);
		} else {
			time_str = "--:";
		}
	}

	// Minutes.
	const int minutes = int(p_time / 60.0f);
	p_time = Math::fmod(p_time, 60.0f);
	if (minutes != 0 || p_includes_empty_minutes) {
		if (!is_undefined) {
			time_str = vformat("%s%02d:", time_str, minutes);
		} else {
			time_str += "--:";
		}
	}

	// Seconds.
	const int seconds = int(p_time);
	if (!is_undefined) {
		time_str = vformat("%s%02d", time_str, seconds);
	} else {
		time_str += "--";
	}

	if (p_includes_ms) {
		// Milliseconds.
		const int milliseconds =
				int(Math::fmod((p_time - seconds) * 1000.0f, 1000.0f));
		if (!is_undefined) {
			time_str = vformat("%s.%03d", time_str, milliseconds);
		} else {
			time_str += ".---";
		}
	}

	return time_str;
}

String SnoreCoreUtils::get_vector_string(
		const Vector2 &p_vector,
		int p_decimal_place_count) {
	return vformat(
			"(%.*f,%.*f)", p_decimal_place_count, p_vector.x,
			p_decimal_place_count, p_vector.y);
}

String SnoreCoreUtils::get_spaces(int p_count) {
	ENSURE(p_count <= 60, "Space count must not exceed 60");
	return String("                                                           "
				  " ")
			.substr(0, p_count);
}

String SnoreCoreUtils::pad_string(
		const String &p_string,
		int p_length,
		bool p_pads_on_right,
		bool p_allows_longer_strings) {
	ENSURE(p_allows_longer_strings || p_string.length() <= p_length,
		   "String length must not exceed target length");
	int spaces_count = p_length - p_string.length();
	if (spaces_count <= 0) {
		return p_string;
	} else {
		const String padding = get_spaces(spaces_count);
		if (p_pads_on_right) {
			return vformat("%s%s", p_string, padding);
		} else {
			return vformat("%s%s", padding, p_string);
		}
	}
}

String SnoreCoreUtils::resize_string(
		const String &p_string,
		int p_length,
		bool p_pads_on_right) {
	if (p_string.length() > p_length) {
		return p_string.substr(0, p_length);
	} else if (p_string.length() < p_length) {
		return pad_string(p_string, p_length, p_pads_on_right);
	} else {
		return p_string;
	}
}

void SnoreCoreUtils::take_screenshot() {
	const Error result =
			DirAccess::make_dir_recursive_absolute("user://screenshots");
	if (result != OK) {
		return;
	}

	const Ref<Image> image =
			SnoreCore::get()->get_viewport()->get_texture()->get_image();
	const String path = vformat(
			"user://screenshots/screenshot-%s.png", get_datetime_string());
	const Error status = image->save_png(path);
	if (status != OK) {
		ENSURE(false, "Failed to save screenshot");
	} else {
		Log::print("Took a screenshot: %s", path);
		were_screenshots_taken = true;
	}
}

void SnoreCoreUtils::open_screenshot_folder() {
	OS *os = OS::get_singleton();
	const String path = os->get_user_data_dir() + "/screenshots";
	Log::print("Opening screenshot folder: " + path);
	os->shell_open(path);
}

void SnoreCoreUtils::set_mouse_filter_recursively(
		Node *p_node,
		Control::MouseFilter p_mouse_filter) {
	Array children = p_node->get_children();
	for (int i = 0; i < children.size(); i++) {
		Node *child = Object::cast_to<Node>(children[i]);
		if (child) {
			Control *control = Object::cast_to<Control>(child);
			if (control) {
				Button *button = Object::cast_to<Button>(control);
				Slider *slider = Object::cast_to<Slider>(control);
				if (!button && !slider) {
					control->set_mouse_filter(p_mouse_filter);
				}
			}
			set_mouse_filter_recursively(child, p_mouse_filter);
		}
	}
}

int SnoreCoreUtils::get_node_vscroll_position(
		ScrollContainer *p_scroll_container,
		Control *p_control,
		int p_offset) {
	const Vector2 scroll_container_global_position =
			p_scroll_container->get_global_position();
	const Vector2 control_global_position = p_control->get_global_position();
	const int vscroll_position = control_global_position.y -
			scroll_container_global_position.y +
			p_scroll_container->get_v_scroll() + p_offset;
	const int max_vscroll_position =
			p_scroll_container->get_v_scroll_bar()->get_max();
	return Math::min(vscroll_position, max_vscroll_position);
}

int64_t SnoreCoreUtils::get_instance_id_or_not(Object *p_object) {
	return p_object != nullptr ? p_object->get_instance_id() : -1;
}

Array SnoreCoreUtils::get_all_nodes_in_group(const StringName &p_group_name) {
	return SnoreCore::get()->get_scene_tree()->get_nodes_in_group(p_group_name);
}

Node *SnoreCoreUtils::get_node_in_group(const StringName &p_group_name) {
	Array nodes = SnoreCore::get()->get_scene_tree()->get_nodes_in_group(
			p_group_name);
	ENSURE(nodes.size() == 1, "Expected exactly one node in group");
	return Object::cast_to<Node>(nodes[0]);
}

Variant SnoreCoreUtils::get_property_value_from_scene_state_node(
		SceneState *p_state,
		int p_node_index,
		const StringName &p_property_name,
		bool p_expects_a_result) {
	for (int property_index = 0;
		 property_index < p_state->get_node_property_count(p_node_index);
		 property_index++) {
		if (p_state->get_node_property_name(p_node_index, property_index) ==
			p_property_name) {
			return p_state->get_node_property_value(
					p_node_index, property_index);
		}
	}
	ENSURE(!p_expects_a_result, "Expected to find property but did not");
	return Variant();
}

bool SnoreCoreUtils::check_whether_sub_classes_are_tools(Object *p_object) {
	Ref<Script> script = p_object->get_script();
	while (script.is_valid()) {
		if (!script->is_tool()) {
			return false;
		}
		script = script->get_base_script();
	}
	return true;
}

bool SnoreCoreUtils::is_running_in_isolated_scene_mode() {
	const StringName main_scene = ProjectSettings::get_singleton()->get_setting(
			"application/run/main_scene");
	const StringName root_scene = SnoreCore::get()
										  ->get_scene_tree()
										  ->get_current_scene()
										  ->get_scene_file_path();
	if (root_scene == main_scene) {
		return false;
	}
	const StringName main_scene_path =
			ResourceUID::get_singleton()->get_id_path(
					ResourceUID::get_singleton()->text_to_id(main_scene));
	return root_scene != main_scene_path;
}

StringName SnoreCoreUtils::get_type_string(int p_type) {
	switch (p_type) {
		case Variant::NIL:
			return "TYPE_NIL";
		case Variant::BOOL:
			return "TYPE_BOOL";
		case Variant::INT:
			return "TYPE_INT";
		case Variant::FLOAT:
			return "TYPE_FLOAT";
		case Variant::STRING:
			return "TYPE_STRING";
		case Variant::VECTOR2:
			return "TYPE_VECTOR2";
		case Variant::RECT2:
			return "TYPE_RECT2";
		case Variant::VECTOR3:
			return "TYPE_VECTOR3";
		case Variant::TRANSFORM2D:
			return "TYPE_TRANSFORM2D";
		case Variant::PLANE:
			return "TYPE_PLANE";
		case Variant::QUATERNION:
			return "TYPE_QUATERNION";
		case Variant::AABB:
			return "TYPE_AABB";
		case Variant::BASIS:
			return "TYPE_BASIS";
		case Variant::TRANSFORM3D:
			return "TYPE_TRANSFORM3D";
		case Variant::COLOR:
			return "TYPE_COLOR";
		case Variant::NODE_PATH:
			return "TYPE_NODE_PATH";
		case Variant::RID:
			return "TYPE_RID";
		case Variant::OBJECT:
			return "TYPE_OBJECT";
		case Variant::DICTIONARY:
			return "TYPE_DICTIONARY";
		case Variant::ARRAY:
			return "TYPE_ARRAY";
		case Variant::VARIANT_MAX:
			return "TYPE_MAX";
		default:
			ENSURE(false, p_type + " is not a valid Variant type");
			return "";
	}
}

String SnoreCoreUtils::get_display_name(const Variant &p_object) {
	String display_name;
	if (p_object.get_type() == Variant::STRING ||
		p_object.get_type() == Variant::STRING_NAME) {
		display_name = p_object;
	} else if (p_object.get_type() == Variant::OBJECT) {
		// Try to use a node's file path.
		const Node *node = Object::cast_to<Node>(p_object);
		if (node) {
			const String scene_path = node->get_scene_file_path();
			if (!scene_path.is_empty()) {
				display_name = scene_path;
			}
		}

		// Try to use a resource's name or file path.
		const Resource *resource = Object::cast_to<Resource>(p_object);
		if (resource) {
			const String name = resource->get_name();
			if (!name.is_empty()) {
				display_name = name;
			} else {
				const String path = resource->get_path();
				if (!path.is_empty()) {
					display_name = path;
				}
			}
		}

		// Try to use a node's name.
		if (display_name.is_empty() && node) {
			display_name = node->get_name();
		}
	}

	// If we still don't have a value, use the Object instance ID.
	if (display_name.is_empty()) {
		return String(p_object);
	}

	// Strip any file extension from the display name.
	Ref<RegEx> regex;
	regex.instantiate();
	regex->compile(R"(([a-zA-Z0-9_ \-]*)\.[a-zA-Z0-9_]*$)");
	Ref<RegExMatch> result = regex->search(display_name);

	if (result.is_null()) {
		return display_name;
	}

	return result->get_string(1);
}

void SnoreCoreUtils::_bind_methods() {
	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("ensure", "condition", "message"),
			&SnoreCoreUtils::ensure, DEFVAL(""));

	ClassDB::bind_static_method(
			"SnoreCoreUtils",
			D_METHOD(
					"splice", "result", "start", "delete_count",
					"items_to_insert"),
			&SnoreCoreUtils::splice);
	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("dedup", "array"),
			&SnoreCoreUtils::dedup);
	ClassDB::bind_static_method(
			"SnoreCoreUtils",
			D_METHOD(
					"subtract_nested_arrays", "result", "other",
					"expects_no_missing_matches"),
			&SnoreCoreUtils::subtract_nested_arrays, DEFVAL(false));
	ClassDB::bind_static_method(
			"SnoreCoreUtils",
			D_METHOD(
					"subtract_arrays", "result", "other",
					"expects_no_missing_matches"),
			&SnoreCoreUtils::subtract_arrays, DEFVAL(false));
	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("array_to_set", "array"),
			&SnoreCoreUtils::array_to_set);
	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("cascade_sort", "arr"),
			&SnoreCoreUtils::cascade_sort);

	ClassDB::bind_static_method(
			"SnoreCoreUtils",
			D_METHOD("translate_polyline", "vertices", "translation"),
			&SnoreCoreUtils::translate_polyline);

	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("clear_children", "node"),
			&SnoreCoreUtils::clear_children);

	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("ease_name_to_param", "name"),
			&SnoreCoreUtils::ease_name_to_param);
	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("ease_by_name", "progress", "ease_name"),
			&SnoreCoreUtils::ease_by_name);

	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("is_num", "v"), &SnoreCoreUtils::is_num);
	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("floor_vector", "v"),
			&SnoreCoreUtils::floor_vector);
	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("ceil_vector", "v"),
			&SnoreCoreUtils::ceil_vector);
	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("round_vector", "v"),
			&SnoreCoreUtils::round_vector);

	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("mix", "values", "weights"),
			&SnoreCoreUtils::mix);
	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("mix_colors", "colors", "weights"),
			&SnoreCoreUtils::mix_colors);

	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("get_datetime_string"),
			&SnoreCoreUtils::get_datetime_string);
	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("get_time_string"),
			&SnoreCoreUtils::get_time_string);
	ClassDB::bind_static_method(
			"SnoreCoreUtils",
			D_METHOD(
					"get_time_string_from_seconds", "time", "includes_ms",
					"includes_empty_hours", "includes_empty_minutes"),
			&SnoreCoreUtils::get_time_string_from_seconds, DEFVAL(false),
			DEFVAL(true), DEFVAL(true));

	ClassDB::bind_static_method(
			"SnoreCoreUtils",
			D_METHOD("get_vector_string", "vector", "decimal_place_count"),
			&SnoreCoreUtils::get_vector_string, DEFVAL(2));
	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("get_spaces", "count"),
			&SnoreCoreUtils::get_spaces);
	ClassDB::bind_static_method(
			"SnoreCoreUtils",
			D_METHOD(
					"pad_string", "string", "length", "pads_on_right",
					"allows_longer_strings"),
			&SnoreCoreUtils::pad_string, DEFVAL(true), DEFVAL(false));
	ClassDB::bind_static_method(
			"SnoreCoreUtils",
			D_METHOD("resize_string", "string", "length", "pads_on_right"),
			&SnoreCoreUtils::resize_string, DEFVAL(true));

	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("take_screenshot"),
			&SnoreCoreUtils::take_screenshot);
	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("open_screenshot_folder"),
			&SnoreCoreUtils::open_screenshot_folder);

	ClassDB::bind_static_method(
			"SnoreCoreUtils",
			D_METHOD("set_mouse_filter_recursively", "node", "mouse_filter"),
			&SnoreCoreUtils::set_mouse_filter_recursively);

	ClassDB::bind_static_method(
			"SnoreCoreUtils",
			D_METHOD(
					"get_node_vscroll_position", "scroll_container", "control",
					"offset"),
			&SnoreCoreUtils::get_node_vscroll_position, DEFVAL(0));

	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("get_instance_id_or_not", "object"),
			&SnoreCoreUtils::get_instance_id_or_not);

	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("get_all_nodes_in_group", "group_name"),
			&SnoreCoreUtils::get_all_nodes_in_group);
	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("get_node_in_group", "group_name"),
			&SnoreCoreUtils::get_node_in_group);

	ClassDB::bind_static_method(
			"SnoreCoreUtils",
			D_METHOD(
					"get_property_value_from_scene_state_node", "state",
					"node_index", "property_name", "expects_a_result"),
			&SnoreCoreUtils::get_property_value_from_scene_state_node,
			DEFVAL(false));

	ClassDB::bind_static_method(
			"SnoreCoreUtils",
			D_METHOD("check_whether_sub_classes_are_tools", "object"),
			&SnoreCoreUtils::check_whether_sub_classes_are_tools);
	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("is_running_in_isolated_scene_mode"),
			&SnoreCoreUtils::is_running_in_isolated_scene_mode);

	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("get_type_string", "type"),
			&SnoreCoreUtils::get_type_string);
	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("get_display_name", "object"),
			&SnoreCoreUtils::get_display_name);

	ClassDB::bind_static_method(
			"SnoreCoreUtils", D_METHOD("get_were_screenshots_taken"),
			&SnoreCoreUtils::get_were_screenshots_taken);

	BIND_CONSTANT(MAX_INT);
}
