#include "snore_core/internal/ref_utils.h"

using namespace godot;

#include <godot_cpp/variant/callable.hpp>

bool godot::is_valid(const Object *p_object) {
	return p_object && ObjectDB::get_instance(p_object->get_instance_id());
}

bool godot::is_valid(const Callable &p_callable) {
	return p_callable.is_valid();
}
