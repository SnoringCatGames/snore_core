#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#include "snore_core/internal/test_utils.h"

#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <vector>

namespace godot {

// FIXME: Use this for various significant framework events.
#define _RAINBOW_BAR                                                           \
	"[color=red]=[/color][color=orange]=[/color][color=yellow]=[/color]"       \
	"[color=green]=[/color][color=blue]=[/color][color=purple]=[/color]"
#define _REVERSE_RAINBOW_BAR                                                   \
	"[color=purple]=[/color][color=blue]=[/color][color=green]=[/color]"       \
	"[color=yellow]=[/color][color=orange]=[/color][color=red]=[/color]"

namespace Log {

namespace Internal {
void print(const String &p_message);
void warning(const String &p_message);
void error(const String &p_message);
void error_skip_assert(const String &p_message);
} // namespace Internal

template <typename... VarArgs>
void debug(const String &p_message = String(), const VarArgs... p_args) {
#ifdef DEBUG_ENABLED
	const String message = vformat(p_message, p_args...);
	Internal::print(message);
#endif // DEBUG_ENABLED
}

template <typename... VarArgs>
void print(const String &p_message = String(), const VarArgs... p_args) {
	const String message = vformat(p_message, p_args...);
	Internal::print(message);
}

template <typename... VarArgs>
void warning(const String &p_message, const VarArgs... p_args) {
	const String message = vformat(p_message, p_args...);
	Internal::warning(message);
}

template <typename... VarArgs>
void error(const String &p_message, const VarArgs... p_args) {
	const String message = vformat(p_message, p_args...);
	Internal::error(message);
}

template <typename... VarArgs>
void error_skip_assert(const String &p_message, const VarArgs... p_args) {
	const String message = vformat(p_message, p_args...);
	Internal::error_skip_assert(message);
}

void empty_line();

void stack_trace();

void print_rich(const String &p_message);

void print_with_color(const String &p_message, const StringName &p_color);

} // namespace Log

String get_stack_trace();

// - DEBUG_BREAK pauses execution if this isn't a release version of the
//   Surfacer framework.
// - DEBUG_BREAK_OR_FALSE allows us to use breakpoints in the ENSURE
//   macro, which relies on the comma operator (otherwise, the compiler would
//   complain about two adjacent commas with no operand in between).
#ifdef SC_CI_ENABLED
// Disable breakpoints when running in continuous integration.
#define DEBUG_BREAK()
#define DEBUG_BREAK_OR_FALSE() false
#else
#ifdef SC_DEV_ENABLED
#ifdef _MSC_VER
#define DEBUG_BREAK() __debugbreak()
#else
#define DEBUG_BREAK() __builtin_debugtrap()
#endif // _MSC_VER
#define DEBUG_BREAK_OR_FALSE() DEBUG_BREAK()
#else
// Disable breakpoints when running in release mode.
#define DEBUG_BREAK()
#define DEBUG_BREAK_OR_FALSE() false
#endif // SC_DEV_ENABLED
#endif // SC_CI_ENABLED

void report_ensure(
		const char *p_function,
		const char *p_file,
		int p_line,
		const char *p_condition,
		const String &p_message = String()) {
	const String message_delimiter = p_message.is_empty() ? "" : "\n";
	const String message_formatted = vformat(
			"%s [%s:%s]\nENSURE failed `%s` is false.%s%s", p_function, p_file,
			p_line, _STR(p_condition), message_delimiter, p_message);
#ifdef SC_TESTS_ENABLED
	godot::TestUtilsInternal::recent_ensures.push_back(message_formatted);
#endif // SC_TESTS_ENABLED
	Log::error_skip_assert(message_formatted);
	Log::stack_trace();
	// FIXME: LEFT OFF HERE: Remove this after verifying the format of the new
	// 						 string above.
	// ::godot::_err_print_error(
	// 		p_function, p_file, p_line,
	// 		"ENSURE failed  \"" _STR(p_condition) "\" is false.", p_message);
	// ::godot::_err_flush_stdout();
}

// Ensures `m_cond` is true.
// - If `m_cond` is false, this prints `m_msg`, pauses execution, and returns
//   false.
// - If `m_cond` is true, this returns true.
// - Use `CHECK` instead if the error is unrecoverable.
#ifdef DEBUG_ENABLED
#define ENSURE(m_cond, m_msg)                                                  \
	(unlikely(!(m_cond)) ? (report_ensure(                                     \
									FUNCTION_STR, __FILE__, __LINE__,          \
									_STR(m_cond), m_msg),                      \
							DEBUG_BREAK_OR_FALSE(), false)                     \
						 : true)
#else
#define ENSURE(m_cond, m_msg) (m_cond)
#endif

#ifdef DEBUG_ENABLED
#define ENSURE_SIMPLE(m_cond)                                                  \
	(unlikely(!(m_cond))                                                       \
			 ? (report_ensure(FUNCTION_STR, __FILE__, __LINE__, _STR(m_cond)), \
				DEBUG_BREAK_OR_FALSE(), false)                                 \
			 : true)
#else
#define ENSURE_SIMPLE(m_cond) (m_cond)
#endif

// This checks whether the condition is true. If not, the program will crash.
// Use `ENSURE` instead, if the error is recoverable.
#ifdef DEBUG_ENABLED
#define CHECK(m_cond, m_msg) CRASH_COND_MSG(!(m_cond), m_msg)
#else
#define CHECK(m_cond, m_msg)
#endif // DEBUG_ENABLED

#ifdef DEBUG_ENABLED
#define CHECK_SIMPLE(m_cond) CRASH_COND(!(m_cond))
#else
#define CHECK_SIMPLE(m_cond)
#endif // DEBUG_ENABLED

// FIXME: Move this to another file.
Variant lerp(Variant p_start, Variant p_end, float p_progress) {
	const Variant::Type start_type = p_start.get_type();
	const Variant::Type final_type = p_end.get_type();
	ENSURE_SIMPLE(start_type == final_type);
	switch (start_type) {
		case Variant::FLOAT:
			return Math::lerp((float)p_start, (float)p_end, p_progress);
		case Variant::INT:
			return Math::lerp(
					(float)(int)p_start, (float)(int)p_end, p_progress);
		case Variant::VECTOR2:
			return ((Vector2)p_start).lerp((Vector2)p_end, p_progress);
		case Variant::VECTOR3:
			return ((Vector3)p_start).lerp((Vector3)p_end, p_progress);
		case Variant::VECTOR4:
			return ((Vector4)p_start).lerp((Vector4)p_end, p_progress);
		case Variant::QUATERNION:
			return ((Quaternion)p_start).slerp((Quaternion)p_end, p_progress);
		case Variant::TRANSFORM2D:
			return ((Transform2D)p_start)
					.interpolate_with((Transform2D)p_end, p_progress);
		case Variant::TRANSFORM3D:
			return ((Transform3D)p_start)
					.interpolate_with((Transform3D)p_end, p_progress);
		case Variant::BASIS:
			return ((Basis)p_start).slerp((Basis)p_end, p_progress);
		default:
			// For unsupported types, just use initial or final value based
			// on progress
			return p_progress < 0.5f ? p_start : p_end;
	}
}

} //namespace godot

#endif // DEBUG_UTILS_H
