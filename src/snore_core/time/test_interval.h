#ifndef TEST_INTERVAL_H
#define TEST_INTERVAL_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/time/interval.h"

#include "snore_core/internal/test_utils.h"

#include <gtest/gtest.h>

namespace godot {

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

class IntervalTest : public SnoreCoreTest {
protected:
	void BeforeEach() override { interval = memnew(Interval); }

	void AfterEach() override { memdelete(interval); }

	Interval *interval;

private:
	void dummy_callback() {
		// Dummy callback for testing
	}
};

TEST_F(IntervalTest, InitialState) {
	// Test that a new interval has expected initial values.
	EXPECT_EQ(interval->get_id(), 0);
	EXPECT_EQ(interval->get_parent(), nullptr);
}

TEST_F(IntervalTest, Initialize) {
	// Test interval initialization.
	Array arguments;
	arguments.push_back("test");

	Callable callback = callable_mp(this, &IntervalTest::dummy_callback);
	interval->initialize(this, 0, callback, 1.0, arguments);

	EXPECT_EQ(interval->get_parent(), this);
	EXPECT_GE(interval->get_id(), 0);
}

TEST_F(IntervalTest, HasReachedNextTriggerTimePlaceholder) {
	// Test the placeholder behavior for has_reached_next_trigger_time.
	// Since we don't have a real time tracker, this should return false.
	Array arguments;
	Callable callback = callable_mp(this, &IntervalTest::dummy_callback);
	interval->initialize(this, 0, callback, 1.0, arguments);

	EXPECT_EQ(interval->get_has_reached_next_trigger_time(), false);
}

TEST_F(IntervalTest, Trigger) {
	// Test that trigger doesn't crash (even though callback may not work in
	// this context).
	Array arguments;
	Callable callback; // Invalid callable
	interval->initialize(this, 0, callback, 1.0, arguments);

	// This should not crash
	interval->trigger();
}

} // namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_INTERVAL_H
