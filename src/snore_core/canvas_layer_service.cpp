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

const std::vector<Ref<CanvasLayerConfig>> CanvasLayerService::
		get_layer_configs() {
	static const std::vector<Ref<CanvasLayerConfig>> configs = {
		set_up_ref<CanvasLayerConfig>(
				CanvasLayerName::utils(),
				Node::ProcessMode::PROCESS_MODE_ALWAYS),
		set_up_ref<CanvasLayerConfig>(
				CanvasLayerName::top(), Node::ProcessMode::PROCESS_MODE_ALWAYS),
		set_up_ref<CanvasLayerConfig>(
				CanvasLayerName::notifications(),
				Node::ProcessMode::PROCESS_MODE_ALWAYS),
		set_up_ref<CanvasLayerConfig>(
				CanvasLayerName::super_hud(),
				Node::ProcessMode::PROCESS_MODE_ALWAYS),
		set_up_ref<CanvasLayerConfig>(
				CanvasLayerName::screens(),
				Node::ProcessMode::PROCESS_MODE_ALWAYS),
		set_up_ref<CanvasLayerConfig>(
				CanvasLayerName::hud(),
				Node::ProcessMode::PROCESS_MODE_PAUSABLE),
		set_up_ref<CanvasLayerConfig>(
				CanvasLayerName::annotations(),
				Node::ProcessMode::PROCESS_MODE_PAUSABLE),
		set_up_ref<CanvasLayerConfig>(
				CanvasLayerName::game(),
				Node::ProcessMode::PROCESS_MODE_PAUSABLE),
	};
	return configs;
}

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
	const std::vector<Ref<CanvasLayerConfig>> layer_configs =
			get_layer_configs();

	for (int index = 0; index < layer_configs.size(); index++) {
		const Ref<CanvasLayerConfig> &config = layer_configs[index];
		const int z_index = layer_configs.size() - index;

		CanvasLayer *layer = memnew(CanvasLayer);
		layer->set_name("Layer_" + config->get_name());
		layer->set_process_mode(config->get_process_mode());
		layer->set_layer(z_index);
		node->add_child(layer);
		layers.emplace(config->get_name(), layer);
	}
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
