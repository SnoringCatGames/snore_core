#include "snore_core/internal/debug_utils.h"

#include "snore_core/internal/string_utils.h"
#include "snore_core/internal/test_utils.h"
#include "snore_core/log_service.h"

#include <godot_cpp/variant/variant.hpp>

using namespace godot;

void godot::Log::Internal::print(const String &p_message) {
	LogService *log_service = LogService::get_maybe();
	if (log_service) {
		log_service->print(p_message);
	} else {
	}
}

void godot::Log::Internal::warning(const String &p_message) {
	LogService *log_service = LogService::get_maybe();
	if (log_service) {
		log_service->print(p_message);
	} else {
	}
}

void godot::Log::Internal::error_skip_assert(const String &p_message) {
	LogService *log_service = LogService::get_maybe();
	if (log_service) {
		log_service->error_skip_assert(p_message);
	} else {
	}
}

void godot::Log::empty_line() {
	LogService *log_service = LogService::get_maybe();
	if (log_service) {
		log_service->print();
	} else {
	}
}

void godot::Log::stack_trace() {
	LogService *log_service = LogService::get_maybe();
	if (log_service) {
		log_service->error_skip_assert(get_stack_trace());
	} else {
	}
}

void godot::Log::print_rich(const String &p_message) {
	LogService *log_service = LogService::get_maybe();
	if (log_service) {
		log_service->print_rich(p_message);
	} else {
	}
}

void godot::Log::print_with_color(
		const String &p_message,
		const StringName &p_color) {
	print_rich(vformat("[color=%s]%s[/color]", p_color, p_message));
}

#if DEBUG_ENABLED
#ifdef _MSC_VER
// clang-format off
#include <windows.h>
#include <dbghelp.h>
// clang-format on
#pragma comment(lib, "dbghelp.lib")
String godot::get_stack_trace() {
	void *stack[80];
	USHORT frames = CaptureStackBackTrace(0, 80, stack, NULL);

	SymInitialize(GetCurrentProcess(), NULL, TRUE);

	String result;

	// Skip i=0, since that's this function.
	for (USHORT i = 1; i < frames; ++i) {
		DWORD64 address = (DWORD64)stack[i];

		DWORD64 displacement = 0;
		char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
		PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		symbol->MaxNameLen = MAX_SYM_NAME;

		result += "    ";

		// result += vformat("[%d] ", i);

		if (SymFromAddr(GetCurrentProcess(), address, &displacement, symbol)) {
			result += symbol->Name;
			// if (displacement > 0) {
			// 	result += vformat(" + 0x%x", (uint32_t)displacement);
			// }

			// Get line information
			IMAGEHLP_LINE64 lineInfo;
			lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
			DWORD displacementLine = 0;
			if (SymGetLineFromAddr64(
						GetCurrentProcess(), address, &displacementLine,
						&lineInfo)) {
				result +=
						vformat(" (%s:%d)", lineInfo.FileName,
								(int)lineInfo.LineNumber);
			}
		} else {
			result += vformat("0x%x", (uintptr_t)stack[i]);
		}
		result += "\n";
	}
	SymCleanup(GetCurrentProcess());

	return result;
}
#else
#include <execinfo.h>
String godot::get_stack_trace() {
	const int max_trace_size = 80;
	void *trace[max_trace_size] = { 0 };

	const int trace_size = backtrace(trace, max_trace_size);
	char **strings = backtrace_symbols(trace, trace_size);

	if (!strings) {
		return String("Stack trace unavailable");
	}

	String result = join_strings(strings, trace_size, "\n    ");

	free(strings);

	return result;
}
#endif // _MSC_VER
#else
String godot::get_stack_trace() { return String(); }
#endif // DEBUG_ENABLED

void godot::report_ensure(
		const char *p_function,
		const char *p_file,
		int p_line,
		const char *p_condition,
		const String &p_message) {
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

Variant godot::lerp(Variant p_start, Variant p_end, float p_progress) {
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
