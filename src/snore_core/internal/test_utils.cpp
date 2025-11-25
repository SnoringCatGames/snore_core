#ifdef SC_TESTS_ENABLED

#include "snore_core/internal/test_utils.h"

#include "snore_core/internal/debug_utils.h"

using namespace godot;

std::vector<String> godot::TestUtilsInternal::recent_ensures = {};

bool godot::TestUtilsInternal::was_an_ensure_expected = false;

void godot::clear_recent_ensures() {
	TestUtilsInternal::recent_ensures.clear();
	TestUtilsInternal::was_an_ensure_expected = false;
}

void SnoreCoreTest::TearDown() {
	if (!TestUtilsInternal::was_an_ensure_expected) {
		EXPECT_TRUE(TestUtilsInternal::recent_ensures.empty());
	}
	ENABLE_ENSURE_BREAKPOINTS();
	clear_recent_ensures();
	AfterEach();
}

#endif // SC_TESTS_ENABLED
