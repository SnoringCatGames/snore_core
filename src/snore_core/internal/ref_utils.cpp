#include "snore_core/internal/ref_utils.h"

#include "snore_core/snore_core_main_module.h"

using namespace godot;

#include <godot_cpp/variant/callable.hpp>

bool godot::is_valid(const Object *p_object) {
	const SnoreCore *snore_core = SnoreCore::get_maybe();
	// Accessing get_instance_id too early can cause a crash.
	const bool is_engine_initialized =
			snore_core && snore_core->get_is_set_up_finished();
	return is_engine_initialized && p_object &&
			ObjectDB::get_instance(p_object->get_instance_id());
}

bool godot::is_valid(const Callable &p_callable) {
	return p_callable.is_valid();
}
