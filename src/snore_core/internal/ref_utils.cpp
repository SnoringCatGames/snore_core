#include "snore_core/internal/ref_utils.h"

using namespace godot;

#include <godot_cpp/variant/callable.hpp>

bool godot::is_valid(const Callable &p_callable) {
	return p_callable.is_valid();
}
