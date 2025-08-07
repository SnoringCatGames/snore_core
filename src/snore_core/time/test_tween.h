#ifndef TEST_TWEEN_H
#define TEST_TWEEN_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/time/tween.h"

#include "snore_core/internal/test_utils.h"

#include <gtest/gtest.h>

namespace godot {

class TweenTest : public SnoreCoreTest {
protected:
	void BeforeEach() override { tween = memnew(SnoreCoreTween); }

	void AfterEach() override { memdelete(tween); }

	SnoreCoreTween *tween;

private:
	void dummy_callback() {
		// Dummy callback for testing
	}
};

// FIXME: LEFT OFF HERE: Write tests.

} // namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_TWEEN_H
