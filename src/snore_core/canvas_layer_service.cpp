#include "snore_core/canvas_layer_service.h"

#include "snore_core/canvas_layer_config.h"
#include "snore_core/canvas_layer_name.h"
#include "snore_core/internal/debug_utils.h"
#include "snore_core/log_service.h"

#include <godot_cpp/classes/canvas_layer.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

namespace {
struct LayerConfig {
	std::string name;
	int z_index = 0;
	Node::ProcessMode process_mode = Node::PROCESS_MODE_INHERIT;
};
} // namespace

const std::vector<LayerConfig> default_layer_configs = {
	{ "top", 8, Node::PROCESS_MODE_ALWAYS },
	{ "utils", 7, Node::PROCESS_MODE_ALWAYS },
	{ "notifications", 6, Node::PROCESS_MODE_ALWAYS },
	{ "super_hud", 5, Node::PROCESS_MODE_ALWAYS },
	{ "screens", 4, Node::PROCESS_MODE_ALWAYS },
	{ "hud", 3, Node::PROCESS_MODE_PAUSABLE },
	{ "annotations", 2, Node::PROCESS_MODE_PAUSABLE },
	{ "game", 1, Node::PROCESS_MODE_PAUSABLE },
};

void CanvasLayerService::set_up() {
	create_root_node();
	create_canvas_layers();
}

void CanvasLayerService::reset() {
	if (is_valid(node)) {
		node->queue_free();
	}
	node = nullptr;
}

void CanvasLayerService::create_root_node() {
	SceneTree *tree = SnoreCore::get()->get_scene_tree();
	if (!tree) {
		return;
	}
	Window *root = tree->get_root();
	if (!root) {
		return;
	}

	node = memnew(Container);
	node->set_name("CanvasLayers");
	root->add_child(node);

	// Make the container fill the screen.
	node->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	node->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	node->set_v_size_flags(Control::SIZE_EXPAND_FILL);
}

void CanvasLayerService::create_canvas_layers() {
	for (int index = 0; index < default_layer_configs.size(); index++) {
		const LayerConfig &raw_config = default_layer_configs[index];
		const StringName name = StringName(raw_config.name.c_str());
		CanvasLayerConfig config;
		config.set_up(name, raw_config.z_index, raw_config.process_mode);
		add_layer(config);
	}
}

void CanvasLayerService::add_layer(const CanvasLayerConfig &p_config) {
	CanvasLayer *layer = memnew(CanvasLayer);
	layer->set_name("Layer_" + String(p_config.get_name()));
	layer->set_process_mode(p_config.get_process_mode());
	layer->set_layer(p_config.get_z_index());

	node->add_child(layer);
	layers.emplace(p_config.get_name(), layer);
}

void CanvasLayerService::add_to_layer(
		const StringName &p_layer_name,
		Node *p_node) {
	if (!ENSURE(layers.find(p_layer_name) != layers.end(),
				vformat("Invalid CanvasLayer: %s", p_layer_name))) {
		return;
	}

	layers[p_layer_name]->add_child(p_node);
}

void CanvasLayerService::remove_from_layer(
		const StringName &p_layer_name,
		Node *p_node) {
	if (!ENSURE(layers.find(p_layer_name) != layers.end(),
				vformat("Invalid CanvasLayer: %s", p_layer_name))) {
		return;
	}

	layers[p_layer_name]->remove_child(p_node);
}

void CanvasLayerService::_bind_methods() {
	ClassDB::bind_method(
			D_METHOD("add_to_layer", "layer_name", "node"),
			&CanvasLayerService::add_to_layer);
	ClassDB::bind_method(
			D_METHOD("remove_from_layer", "layer_name", "node"),
			&CanvasLayerService::remove_from_layer);
}
