#include "snore_core/snore_core_submodule.h"

#include "snore_core/log_service.h"

using namespace godot;

void SnoreCoreSubmodule::set_up_base() {
	instantiate_node();
	set_up();
	LogService::get()->report_submodule_initialized(get_name());
}

void SnoreCoreSubmodule::reset_base() {
	reset_node();
	reset();
}
