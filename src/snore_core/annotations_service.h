#ifndef ANNOTATIONS_SERVICE_H
#define ANNOTATIONS_SERVICE_H

#include "snore_core/snore_core_main_module.h"
#include "snore_core/snore_core_submodule.h"

#include <godot_cpp/classes/node2d.hpp>

namespace godot {

class AnnotationsService : public SnoreCoreSubmoduleWithNode {
	GDCLASS(AnnotationsService, SnoreCoreSubmoduleWithNode)
	SC_SUBMODULE_CLASS(AnnotationsService, SnoreCore)

public:
	AnnotationsService() = default;
	~AnnotationsService() = default;

protected:
	static void _bind_methods();
};

} //namespace godot

#endif
