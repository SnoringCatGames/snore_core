#ifndef LOGGER_H
#define LOGGER_H

#include "snore_core/circular_buffer.h"
#include "snore_core/snore_core_main_module.h"
#include "snore_core/snore_core_submodule.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

namespace godot {

class Logger : public SnoreCoreSubmodule {
	GDCLASS(Logger, SnoreCoreSubmodule)
	SC_SUBMODULE_CLASS(Logger, SnoreCore)

public:
	static const constexpr int32_t MAX_LOG_COUNT = 200;

	Logger() = default;
	~Logger() = default;

	template <typename... VarArgs>
	void print(const String &p_message = String(), const VarArgs... p_args) {
		print_helper(vformat(p_message, p_args...), true);
	}

	void print_rich(const String &p_message);

	void print_with_color(const Variant &p_message, const String &p_color);

	template <typename... VarArgs>
	void warning(const String &p_message, const VarArgs... p_args) {
		warning_helper(vformat(p_message, p_args...));
	}

	template <typename... VarArgs>
	void error(const String &p_message, const VarArgs... p_args) {
		error_helper(vformat(p_message, p_args...), true);
	}

	template <typename... VarArgs>
	void error_skip_assert(const String &p_message, const VarArgs... p_args) {
		error_helper(vformat(p_message, p_args...), false);
	}

	void report_submodule_initialized(const StringName &p_name);

	const Ref<CircularBuffer> get_recent_logs() const { return recent_logs; }

protected:
	static void _bind_methods();

private:
	void script_print(const Variant &p_message = Variant());
	void script_warning(const Variant &p_message);
	void script_error(const Variant &p_message);
	void script_error_skip_assert(const Variant &p_message);

	void print_skip_console(const String &p_message = String()) {
		print_helper(p_message, false, false);
	}

	void print_helper(
			const String &p_message,
			bool p_print_to_console,
			bool p_is_rich);
	void warning_helper(const String &p_message);
	void error_helper(const String &p_message, bool p_should_assert);

	void print_front_matter();

	static String prepend_time(const String &p_message);

	Ref<CircularBuffer> recent_logs;
};

} // namespace godot

#endif // LOGGER_H
