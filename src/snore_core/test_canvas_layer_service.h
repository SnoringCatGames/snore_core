#ifndef TEST_CANVAS_LAYER_SERVICE_H
#define TEST_CANVAS_LAYER_SERVICE_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/canvas_layer_service.h"

#include "snore_core/internal/test_utils.h"

#include <gtest/gtest.h>

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

namespace godot {

class CanvasLayerServiceTest : public SnoreCoreTest {
protected:
	void BeforeEach() override { service.instantiate(); }

	void AfterEach() override { service.unref(); }

	Ref<CanvasLayerService> service;
};

TEST_F(CanvasLayerServiceTest, CanvasLayerManagement) {
	// Create a test node.
	Ref<Node> test_node;
	test_node.instantiate();
	test_node->set_name("test_node");

	// Test that canvas layers are created.
	// Since create_canvas_layers() now creates default layers,
	// we should be able to add nodes to them.
	service->_enter_tree(); // This should call create_canvas_layers().

	// Test adding to a valid layer (should work with default implementation).
	service->add_to_layer("hud", test_node.ptr());

	// Test adding to invalid layer (should log error but not crash).
	service->add_to_layer("nonexistent_layer", test_node.ptr());

	// Test removing from a valid layer.
	service->remove_from_layer("hud", test_node.ptr());

	// Test removing from invalid layer (should log error but not crash).
	service->remove_from_layer("nonexistent_layer", test_node.ptr());
}

} //namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_CANVAS_LAYER_SERVICE_H
