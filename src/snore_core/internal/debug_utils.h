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

namespace Internal {
extern bool ensure_breakpoint_enabled;
} // namespace Internal

#define DISABLE_ENSURE_BREAKPOINTS()                                           \
	(Internal::ensure_breakpoint_enabled = false)
#define ENABLE_ENSURE_BREAKPOINTS() (Internal::ensure_breakpoint_enabled = true)

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
#define DEBUG_BREAK_OR_FALSE()                                                 \
	(Internal::ensure_breakpoint_enabled ? (DEBUG_BREAK(), false) : false)
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
		const String &p_message = String());

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
Variant lerp(Variant p_start, Variant p_end, float p_progress);

} //namespace godot

#endif // DEBUG_UTILS_H
