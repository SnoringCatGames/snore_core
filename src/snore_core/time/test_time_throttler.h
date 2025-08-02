#ifndef TEST_TIME_THROTTLER_H
#define TEST_TIME_THROTTLER_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/time/time_throttler.h"

#include "snore_core/internal/test_utils.h"

#include <gtest/gtest.h>

namespace godot {

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

class TimeThrottlerTest : public SnoreCoreTest {
protected:
	void BeforeEach() override { throttler = memnew(TimeThrottler); }

	void AfterEach() override { memdelete(throttler); }

	TimeThrottler *throttler;

private:
	void dummy_callback() {
		// Dummy callback for testing
	}
};

TEST_F(TimeThrottlerTest, InitialState) {
	// Test that a new throttler has expected initial values.
	EXPECT_EQ(throttler->get_parent(), nullptr);
}

TEST_F(TimeThrottlerTest, Initialize) {
	// Test throttler initialization.
	Callable callback = callable_mp(this, &TimeThrottlerTest::dummy_callback);
	throttler->initialize(this, 0, callback, 1.0, true);

	EXPECT_EQ(throttler->get_parent(), this);
}

TEST_F(TimeThrottlerTest, GetOnCall) {
	// Test that get_on_call returns a valid callable.
	Callable callback = callable_mp(this, &TimeThrottlerTest::dummy_callback);
	throttler->initialize(this, 0, callback, 1.0, true);

	Callable on_call = throttler->get_on_call();
	EXPECT_TRUE(on_call.is_valid());
}

TEST_F(TimeThrottlerTest, OnCallPlaceholder) {
	// Test that on_call doesn't crash with placeholder implementation.
	Callable callback = callable_mp(this, &TimeThrottlerTest::dummy_callback);
	throttler->initialize(this, 0, callback, 1.0, true);

	// This should not crash
	throttler->on_call();
}

TEST_F(TimeThrottlerTest, Cancel) {
	// Test that cancel doesn't crash.
	Callable callback = callable_mp(this, &TimeThrottlerTest::dummy_callback);
	throttler->initialize(this, 0, callback, 1.0, true);

	// This should not crash
	throttler->cancel();
}

} // namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_TIME_THROTTLER_H
