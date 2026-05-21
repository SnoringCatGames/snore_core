#ifndef CANVAS_LAYER_SERVICE_H
#define CANVAS_LAYER_SERVICE_H

#include "snore_core/canvas_layer_config.h"
#include "snore_core/snore_core_main_module.h"
#include "snore_core/snore_core_submodule.h"

#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/container.hpp>
#include <godot_cpp/templates/hash_map.hpp>

namespace godot {

class CanvasLayerConfig;

class CanvasLayerService : public SnoreCoreSubmodule {
	GDCLASS(CanvasLayerService, SnoreCoreSubmodule)
	SC_SUBMODULE_CLASS(CanvasLayerService, SnoreCore)

public:
	CanvasLayerService() = default;
	virtual ~CanvasLayerService() = default;

	void add_layer(const Ref<CanvasLayerConfig> p_config);
	void add_to_layer(const StringName &p_layer_name, Node *p_node);
	void remove_from_layer(const StringName &p_layer_name, Node *p_node);

protected:
	static void _bind_methods();

private:
	Container *node = nullptr;

	HashMap<StringName, CanvasLayer *> layers;

	void create_root_node();
	void create_canvas_layers();
};

} //namespace godot

#endif // CANVAS_LAYER_SERVICE_H
