#ifndef TEST_TIME_DEBOUNCER_H
#define TEST_TIME_DEBOUNCER_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/time/time_debouncer.h"

#include "snore_core/internal/test_utils.h"

#include <gtest/gtest.h>

namespace godot {

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

class TimeDebouncerTest : public SnoreCoreTest {
protected:
	void BeforeEach() override { debouncer = memnew(TimeDebouncer); }

	void AfterEach() override { memdelete(debouncer); }

	TimeDebouncer *debouncer;

private:
	void dummy_callback() {
		// Dummy callback for testing
	}
};

TEST_F(TimeDebouncerTest, InitialState) {
	// Test that a new debouncer has expected initial values.
	EXPECT_EQ(debouncer->get_parent(), nullptr);
}

TEST_F(TimeDebouncerTest, Initialize) {
	// Test debouncer initialization.
	Callable callback = callable_mp(this, &TimeDebouncerTest::dummy_callback);
	debouncer->initialize(this, 0, callback, 1.0, false);

	EXPECT_EQ(debouncer->get_parent(), this);
}

TEST_F(TimeDebouncerTest, GetOnCall) {
	// Test that get_on_call returns a valid callable.
	Callable callback = callable_mp(this, &TimeDebouncerTest::dummy_callback);
	debouncer->initialize(this, 0, callback, 1.0, false);

	Callable on_call = debouncer->get_on_call();
	EXPECT_TRUE(on_call.is_valid());
}

TEST_F(TimeDebouncerTest, OnCallPlaceholder) {
	// Test that on_call doesn't crash with placeholder implementation.
	Callable callback = callable_mp(this, &TimeDebouncerTest::dummy_callback);
	debouncer->initialize(this, 0, callback, 1.0, false);

	// This should not crash
	debouncer->on_call();
}

TEST_F(TimeDebouncerTest, Cancel) {
	// Test that cancel doesn't crash.
	Callable callback = callable_mp(this, &TimeDebouncerTest::dummy_callback);
	debouncer->initialize(this, 0, callback, 1.0, false);

	// This should not crash
	debouncer->cancel();
}

} // namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_TIME_DEBOUNCER_H
