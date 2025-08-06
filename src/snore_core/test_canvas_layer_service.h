#ifndef TEST_CANVAS_LAYER_SERVICE_H
#define TEST_CANVAS_LAYER_SERVICE_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/canvas_layer_service.h"

#include "snore_core/internal/test_utils.h"

#include <gtest/gtest.h>
#include <godot_cpp/classes/canvas_item.hpp>

namespace godot {

class CanvasLayerServiceTest : public SnoreCoreTest {
protected:
	void BeforeEach() override { service.instantiate(); }

	void AfterEach() override { service.unref(); }

	Ref<CanvasLayerService> service;
};

} //namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_CANVAS_LAYER_SERVICE_H
