#include "snore_core/snore_core_submodule.h"

#include "snore_core/log_service.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/string_name.hpp>

using namespace godot;

void SnoreCoreSubmoduleInternal::add_utility_node(
		Node *p_node,
		const StringName &p_name) {
	SnoreCore::get()->add_utility_node(p_node, p_name);
}

void SnoreCoreSubmodule::set_up_base() {
	instantiate_node();
	set_up();
	LogService::report_submodule_initialized(get_name());
}

void SnoreCoreSubmodule::reset_base() {
	reset_node();
	reset();
}
