#ifdef SC_TESTS_ENABLED

#include "snore_core/internal/test_utils.h"

using namespace godot;

std::vector<String> godot::TestUtilsInternal::recent_ensures = {};

bool godot::TestUtilsInternal::was_an_ensure_expected = false;

#endif // SC_TESTS_ENABLED
