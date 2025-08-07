#ifndef TEST_TIMEOUT_H
#define TEST_TIMEOUT_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/time/timeout.h"

#include "snore_core/internal/test_utils.h"

#include <gtest/gtest.h>

namespace godot {

class TimeoutTest : public SnoreCoreTest {
protected:
	void BeforeEach() override { timeout = memnew(Timeout); }

	void AfterEach() override { memdelete(timeout); }

	Timeout *timeout;

private:
	void dummy_callback() {}
};

TEST_F(TimeoutTest, HasExpiredPlaceholder) {
	// Test the placeholder behavior for has_expired.
	// Since we don't have a real time tracker, this should return false.
	Array arguments;
	Callable callback = callable_mp(this, &TimeoutTest::dummy_callback);
	timeout->initialize(this, 0, callback, 1.0, arguments);

	EXPECT_EQ(timeout->get_has_expired(), false);
}

} // namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_TIMEOUT_H
