#include "snore_core/snore_core_submodule.h"

#include "snore_core/logger.h"

using namespace godot;

void SnoreCoreSubmodule::set_up_base() {
	instantiate_node();
	set_up();
	Logger::get()->report_submodule_initialized(get_name());
}

void SnoreCoreSubmodule::reset_base() {
	reset_node();
	reset();
}
