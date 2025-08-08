#ifndef TEST_INTERVAL_H
#define TEST_INTERVAL_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/time/interval.h"

#include "snore_core/internal/test_utils.h"

#include <gtest/gtest.h>
#include <godot_cpp/variant/callable.hpp>

namespace godot {

class IntervalTest : public SnoreCoreTest {
protected:
	void BeforeEach() override {
		interval = memnew(Interval);
		parent = memnew(Object);
	}

	void AfterEach() override {
		memdelete(interval);
		memdelete(parent);
	}

	Interval *interval;
	Object *parent;
};

TEST_F(IntervalTest, HasReachedNextTriggerTimePlaceholder) {
	// Test the placeholder behavior for has_reached_next_trigger_time.
	// Since we don't have a real time tracker, this should return false.
	Array arguments;
	Callable callback;
	interval->initialize(
			parent, TimeType::APP_PHYSICS, callback, 1.0, arguments);

	EXPECT_EQ(interval->get_has_reached_next_trigger_time(), false);
}

} // namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_INTERVAL_H
