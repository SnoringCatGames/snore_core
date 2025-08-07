#ifndef TEST_TIMEOUT_H
#define TEST_TIMEOUT_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/time/timeout.h"

#include "snore_core/internal/test_utils.h"

#include <gtest/gtest.h>

namespace godot {

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

class TimeoutTest : public SnoreCoreTest {
protected:
	void BeforeEach() override { timeout = memnew(Timeout); }

	void AfterEach() override { memdelete(timeout); }

	Timeout *timeout;

private:
	void dummy_callback() {
		// Dummy callback for testing
	}
};

TEST_F(TimeoutTest, InitialState) {
	// Test that a new timeout has expected initial values.
	EXPECT_EQ(timeout->get_id(), 0);
	EXPECT_EQ(timeout->get_parent(), nullptr);
}

TEST_F(TimeoutTest, Initialize) {
	// Test timeout initialization.
	Array arguments;
	arguments.push_back("test");

	Callable callback = callable_mp(this, &TimeoutTest::dummy_callback);
	timeout->initialize(this, 0, callback, 1.0, arguments);

	EXPECT_EQ(timeout->get_parent(), this);
	EXPECT_GE(timeout->get_id(), 0);
}

TEST_F(TimeoutTest, HasExpiredPlaceholder) {
	// Test the placeholder behavior for has_expired.
	// Since we don't have a real time tracker, this should return false.
	Array arguments;
	Callable callback = callable_mp(this, &TimeoutTest::dummy_callback);
	timeout->initialize(this, 0, callback, 1.0, arguments);

	EXPECT_EQ(timeout->get_has_expired(), false);
}

TEST_F(TimeoutTest, Trigger) {
	// Test that trigger doesn't crash (even though callback may not work in
	// this context).
	Array arguments;
	Callable callback; // Invalid callable
	timeout->initialize(this, 0, callback, 1.0, arguments);

	// This should not crash
	timeout->trigger();
}

} // namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_TIMEOUT_H
