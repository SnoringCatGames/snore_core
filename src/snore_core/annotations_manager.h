#ifndef ANNOTATIONS_MANAGER_H
#define ANNOTATIONS_MANAGER_H

#include "snore_core/snore_core_main_module.h"
#include "snore_core/snore_core_submodule.h"

#include <godot_cpp/classes/node2d.hpp>

namespace godot {

class AnnotationsManager : public SnoreCoreSubmoduleWithNode {
	GDCLASS(AnnotationsManager, SnoreCoreSubmoduleWithNode)
	SC_SUBMODULE_CLASS(AnnotationsManager, SnoreCore)

public:
	AnnotationsManager() = default;
	~AnnotationsManager() = default;

protected:
	static void _bind_methods();
};

} //namespace godot

#endif
