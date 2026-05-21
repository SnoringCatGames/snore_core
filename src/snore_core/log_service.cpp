#include "snore_core/log_service.h"

#include "snore_core/internal/debug_utils.h"
#include "snore_core/snore_core_main_settings.h"
#include "snore_core/snore_core_utils.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void LogService::set_up() {
	recent_logs.instantiate();
	recent_logs->initialize(MAX_LOG_COUNT);

	print_front_matter();
}

void LogService::reset() { recent_logs.unref(); }

void LogService::script_print(const Variant &p_message) {
	LogService *log_service = LogService::get_maybe();
	if (log_service) {
		String message;
		if (p_message.get_type() == Variant::STRING) {
			message = p_message;
		} else {
			message = p_message.stringify();
		}
		log_service->print_helper(message, true, false);
	} else {
	}
}

void LogService::script_warning(const Variant &p_message) {
	LogService *log_service = LogService::get_maybe();
	if (log_service) {
		String message;
		if (p_message.get_type() == Variant::STRING) {
			message = p_message;
		} else {
			message = p_message.stringify();
		}
		log_service->warning_helper(message);
	} else {
	}
}

void LogService::script_error_skip_assert(const Variant &p_message) {
	LogService *log_service = LogService::get_maybe();
	if (log_service) {
		String message;
		if (p_message.get_type() == Variant::STRING) {
			message = p_message;
		} else {
			message = p_message.stringify();
		}
		log_service->error_helper(message, false);
	} else {
	}
}

void LogService::print_rich(const String &p_message) {
	print_helper(p_message, true, true);
}

void LogService::print_with_color(
		const Variant &p_message,
		const String &p_color) {
	print_rich(vformat("[color=%s]%s[/color]", p_color, p_message.stringify()));
}

void LogService::print_helper(
		const String &p_message,
		bool p_print_to_console,
		bool p_is_rich) {
	const String message = prepend_time(p_message);

	if (is_valid(recent_logs)) {
		recent_logs->push(message);
	}

	// FIXME: Integrate with the in-game logs display widget. Refactor this to
	//        broadcast an event though, for the display to listen to.
	// if (is_valid(S) && is_valid(S.super_hud)):
	//     S.logs_display.add_log(message, MAX_LOG_COUNT)

	if (p_print_to_console && !SnoreCore::get_are_tests_running()) {
		if (p_is_rich) {
			UtilityFunctions::print_rich(message);
		} else {
			UtilityFunctions::print(message);
		}
	}
}

void LogService::error_helper(const String &p_message, bool p_should_assert) {
	// FIXME: Integrate with the TimeService submodule.
	// float play_time = S.time.get_play_time() if is_valid(S.time)
	// else -1.0;
	const float play_time = -1.0;

	const String timestamped_message = prepend_time(p_message);
	const String error_message =
			vformat("ERROR:%8.3f; %s", play_time, timestamped_message);

	ERR_PRINT(error_message);
	print_skip_console(
			vformat("**ERROR**:%8.3f; %s", play_time, timestamped_message));

	if (p_should_assert) {
		ENSURE_SIMPLE(false);
	}
}

void LogService::warning_helper(const String &p_message) {
	const String timestamped_message = prepend_time(p_message);
	const String warning_message = vformat("WARNING: %s", timestamped_message);

	WARN_PRINT(warning_message);
	print_skip_console(vformat("**WARNING**: %s", timestamped_message));
}

void LogService::report_submodule_initialized(const StringName &p_name) {
	LogService *log_service = LogService::get_maybe();
	if (log_service) {
		if (SnoreCoreMainSettings::get()->get_log_initialization_events()) {
			log_service->print("[INITIALIZED] %s", p_name);
		}
	} else {
	}
}

void LogService::print_front_matter() {
	print(SnoreCoreUtils::get_datetime_string());

	const Viewport *viewport = SnoreCore::get()->get_viewport();
	const Vector2i viewport_size =
			viewport ? Vector2i(viewport->get_visible_rect().size) : Vector2i();

	const OS *os = OS::get_singleton();
	const String system_info =
			vformat("%s %s (%4d,%4d) ", os->get_name(), os->get_model_name(),
					viewport_size.x, viewport_size.y);

	print(system_info);

	if (SnoreCoreMainSettings::get()->get_log_initialization_events()) {
		print("");
	}
}

String LogService::prepend_time(const String &p_message) {
	return vformat("[%s] %s", SnoreCoreUtils::get_time_string(), p_message);
}

void LogService::_bind_methods() {
	ClassDB::bind_static_method(
			name, D_METHOD("print", "p_message"), &LogService::script_print,
			DEFVAL(Variant()));
	ClassDB::bind_static_method(
			name, D_METHOD("warning", "p_message"),
			&LogService::script_warning);
	// NOTE: We're rebinding this on Log for convenience.
	ClassDB::bind_static_method(
			name, D_METHOD("ensure", "condition", "message"),
			&SnoreCoreUtils::ensure, DEFVAL(""));
	ClassDB::bind_static_method(
			name, D_METHOD("error_skip_assert", "p_message"),
			&LogService::script_error_skip_assert);

	ClassDB::bind_static_method(
			name, D_METHOD("report_submodule_initialized", "p_name"),
			&LogService::report_submodule_initialized, DEFVAL(true));

	BIND_CONSTANT(MAX_LOG_COUNT);
}
