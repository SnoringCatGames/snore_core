#ifndef TEST_TIMEOUT_H
#define TEST_TIMEOUT_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/time/timeout.h"

#include "snore_core/internal/test_utils.h"

#include <gtest/gtest.h>
#include <godot_cpp/variant/callable.hpp>

namespace godot {

class TimeoutTest : public SnoreCoreTest {
protected:
	void BeforeEach() override {
		timeout = memnew(Timeout);
		parent = memnew(Object);
	}

	void AfterEach() override {
		memdelete(timeout);
		memdelete(parent);
	}

	Timeout *timeout;
	Object *parent;
};

TEST_F(TimeoutTest, HasExpiredPlaceholder) {
	// Test the placeholder behavior for has_expired.
	// Since we don't have a real time tracker, this should return false.
	Array arguments;
	Callable callback;
	timeout->initialize(
			parent, TimeType::APP_PHYSICS, callback, 1.0, arguments);

	EXPECT_EQ(timeout->get_has_expired(), false);
}

} // namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_TIMEOUT_H
