#ifndef REGISTRATION_UTILS_H
#define REGISTRATION_UTILS_H

#include <godot_cpp/classes/object.hpp>

namespace godot {

constexpr const char PROCESS_MODE_HINT_STRING[] =
		"INHERIT,PAUSABLE,WHEN_PAUSED,ALWAYS,DISABLED";

constexpr uint64_t PROPERTY_USAGE_EXPORTED_ITEM = PROPERTY_USAGE_STORAGE |
		PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_SCRIPT_VARIABLE;

#define EXPORTED_PROPERTY_INFO(type, name)                                     \
	PropertyInfo(                                                              \
			type, name, PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EXPORTED_ITEM)
#define EXPORTED_PROPERTY_INFO_WITH_HINT(type, name, hint_type, hint_string)   \
	PropertyInfo(                                                              \
			type, name, hint_type, hint_string, PROPERTY_USAGE_EXPORTED_ITEM)

#define BIND_LOGGING_FLAG(m_settings_class, m_category)                        \
	do {                                                                       \
		ClassDB::bind_method(                                                  \
				D_METHOD("get_log_" #m_category),                              \
				&m_settings_class::get_log_##m_category);                      \
		ClassDB::bind_method(                                                  \
				D_METHOD("set_log_" #m_category, "p_value"),                   \
				&m_settings_class::set_log_##m_category);                      \
		ADD_PROPERTY(                                                          \
				EXPORTED_PROPERTY_INFO(Variant::BOOL, "log_" #m_category),     \
				"set_log_" #m_category, "get_log_" #m_category);               \
	} while (false)

} //namespace godot

#endif // REGISTRATION_UTILS_H
