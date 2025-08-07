#ifndef TEST_INTERVAL_H
#define TEST_INTERVAL_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/time/interval.h"

#include "snore_core/internal/test_utils.h"

#include <gtest/gtest.h>

namespace godot {

class IntervalTest : public SnoreCoreTest {
protected:
	void BeforeEach() override { interval = memnew(Interval); }

	void AfterEach() override { memdelete(interval); }

	Interval *interval;

private:
	void dummy_callback() {}
};

TEST_F(IntervalTest, HasReachedNextTriggerTimePlaceholder) {
	// Test the placeholder behavior for has_reached_next_trigger_time.
	// Since we don't have a real time tracker, this should return false.
	Array arguments;
	Callable callback = callable_mp(this, &IntervalTest::dummy_callback);
	interval->initialize(this, 0, callback, 1.0, arguments);

	EXPECT_EQ(interval->get_has_reached_next_trigger_time(), false);
}

} // namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_INTERVAL_H
