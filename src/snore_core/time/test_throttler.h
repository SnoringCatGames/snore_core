#ifndef TEST_THROTTLER_H
#define TEST_THROTTLER_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/time/throttler.h"

#include "snore_core/internal/test_utils.h"

#include <gtest/gtest.h>

namespace godot {

class ThrottlerTest : public SnoreCoreTest {
protected:
	void BeforeEach() override { throttler = memnew(Throttler); }

	void AfterEach() override { memdelete(throttler); }

	Throttler *throttler;

private:
	void dummy_callback() {}
};

} // namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_THROTTLER_H
