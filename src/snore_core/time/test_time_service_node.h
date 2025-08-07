#ifndef TEST_NODE_NODE_H
#define TEST_NODE_NODE_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/time/time_service_node.h"

#include "snore_core/internal/test_utils.h"

#include <gtest/gtest.h>

namespace godot {

class TimeServiceNodeTest : public SnoreCoreTest {
protected:
	void BeforeEach() override {
		node = memnew(TimeServiceNode);
		node->_ready();
	}

	void AfterEach() override { memdelete(node); }

	TimeServiceNode *node;
};

} // namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_NODE_NODE_H
