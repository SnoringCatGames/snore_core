#include "snore_core/snore_core_main_settings.h"

#include "snore_core/internal/registration_utils.h"
#include "snore_core/snore_core_main_module.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

// TODO: Update the demo settings to use the default values from the old
//       manifest.gd.

SC_SETTINGS_CLASS_DEFINITION_ON_MODULE(SnoreCoreMainSettings, SnoreCore)

void SnoreCoreMainSettings::_bind_methods() {
	ClassDB::bind_method(
			D_METHOD("get_debug_time_scale"),
			&SnoreCoreMainSettings::get_debug_time_scale);
	ClassDB::bind_method(
			D_METHOD("set_debug_time_scale", "p_scale"),
			&SnoreCoreMainSettings::set_debug_time_scale);
	ADD_PROPERTY(
			EXPORTED_PROPERTY_INFO_WITH_HINT(
					Variant::FLOAT, "debug_time_scale", PROPERTY_HINT_RANGE,
					"0.5,5.0,0.1"),
			"set_debug_time_scale", "get_debug_time_scale");

	ClassDB::bind_method(
			D_METHOD("get_render_debug_annotations"),
			&SnoreCoreMainSettings::get_render_debug_annotations);
	ClassDB::bind_method(
			D_METHOD("set_render_debug_annotations", "p_value"),
			&SnoreCoreMainSettings::set_render_debug_annotations);
	ADD_PROPERTY(
			EXPORTED_PROPERTY_INFO(Variant::BOOL, "render_debug_annotations"),
			"set_render_debug_annotations", "get_render_debug_annotations");

	ADD_GROUP("Flags", "");
	ClassDB::bind_method(
			D_METHOD("get_dev_mode"), &SnoreCoreMainSettings::get_dev_mode);
	ClassDB::bind_method(
			D_METHOD("set_dev_mode", "p_value"),
			&SnoreCoreMainSettings::set_dev_mode);
	ADD_PROPERTY(
			EXPORTED_PROPERTY_INFO(Variant::BOOL, "dev_mode"), "set_dev_mode",
			"get_dev_mode");

	ADD_SUBGROUP("Logging", "log");
	BIND_LOGGING_FLAG(SnoreCoreMainSettings, snore_core_events);
	BIND_LOGGING_FLAG(SnoreCoreMainSettings, snore_core_events_verbose);
	BIND_LOGGING_FLAG(SnoreCoreMainSettings, initialization_events);

	ADD_GROUP("Advanced", "");
	ClassDB::bind_method(
			D_METHOD("get_user_settings_path"),
			&SnoreCoreMainSettings::get_user_settings_path);
	ClassDB::bind_method(
			D_METHOD("set_user_settings_path", "p_value"),
			&SnoreCoreMainSettings::set_user_settings_path);
	ADD_PROPERTY(
			EXPORTED_PROPERTY_INFO(Variant::STRING_NAME, "user_settings_path"),
			"set_user_settings_path", "get_user_settings_path");
}
