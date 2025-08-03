#ifndef TEST_SNORE_CORE_TIME_H
#define TEST_SNORE_CORE_TIME_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/time/snore_core_time.h"

#include "snore_core/internal/test_utils.h"
#include "snore_core/time/time_type.h"

#include <gtest/gtest.h>

namespace godot {

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

class SnoreCoreTimeTest : public SnoreCoreTest {
protected:
	void BeforeEach() override {
		snore_core_time = memnew(SnoreCoreTime);
		snore_core_time->_init();
		snore_core_time->_ready();
	}

	void AfterEach() override { memdelete(snore_core_time); }

	SnoreCoreTime *snore_core_time;
};

TEST_F(SnoreCoreTimeTest, Constants) {
	// Test that constants are properly defined.
	EXPECT_EQ(SnoreCoreTime::PHYSICS_FPS, 60.0);
	EXPECT_EQ(SnoreCoreTime::PHYSICS_TIME_STEP, 1.0 / 60.0);
}

TEST_F(SnoreCoreTimeTest, InitialState) {
	// Test that a new scaffolder time has expected initial values.
	EXPECT_EQ(snore_core_time->get_time_scale(), 1.0);
	EXPECT_EQ(snore_core_time->get_additional_debug_time_scale(), 1.0);
	EXPECT_EQ(snore_core_time->get_combined_scale(), 1.0);
}

TEST_F(SnoreCoreTimeTest, TimeScaleSetting) {
	// Test setting time scales.
	snore_core_time->_set_time_scale(2.0);
	EXPECT_EQ(snore_core_time->get_time_scale(), 2.0);
	EXPECT_EQ(snore_core_time->get_combined_scale(), 2.0);

	snore_core_time->_set_additional_debug_time_scale(0.5);
	EXPECT_EQ(snore_core_time->get_additional_debug_time_scale(), 0.5);
	EXPECT_EQ(snore_core_time->get_combined_scale(), 1.0);
}

TEST_F(SnoreCoreTimeTest, ScalingFunctions) {
	// Test time scaling functions.
	snore_core_time->_set_time_scale(2.0);

	float duration = 1.0;
	float scaled_duration = snore_core_time->scale_delta(duration);
	EXPECT_EQ(scaled_duration, 2.0);

	float scaled_time_step = snore_core_time->get_scaled_time_step();
	float expected = SnoreCoreTime::PHYSICS_TIME_STEP * 2.0;
	EXPECT_EQ(scaled_time_step, expected);
}

TEST_F(SnoreCoreTimeTest, TaskIdGeneration) {
	// Test that task IDs are generated incrementally.
	int id1 = snore_core_time->get_next_task_id();
	int id2 = snore_core_time->get_next_task_id();
	EXPECT_EQ(id2, id1 + 1);
}

TEST_F(SnoreCoreTimeTest, TimeTrackerAccess) {
	// Test accessing time trackers for different time types.
	TimeTracker *app_tracker = snore_core_time->_get_time_tracker_for_time_type(
			TimeType::APP_PHYSICS);
	TimeTracker *play_tracker =
			snore_core_time->_get_time_tracker_for_time_type(
					TimeType::PLAY_PHYSICS);

	EXPECT_NE(app_tracker, nullptr);
	EXPECT_NE(play_tracker, nullptr);
	EXPECT_NE(app_tracker, play_tracker);
}

TEST_F(SnoreCoreTimeTest, ElapsedTimeKeys) {
	// Test that elapsed time keys are returned correctly.
	StringName physics_key =
			snore_core_time->_get_elapsed_time_key_for_time_type(
					TimeType::APP_PHYSICS);
	StringName clock_key = snore_core_time->_get_elapsed_time_key_for_time_type(
			TimeType::APP_CLOCK);
	StringName frame_key = snore_core_time->_get_elapsed_time_key_for_time_type(
			TimeType::APP_PHYSICS_FRAME_COUNT);

	EXPECT_EQ(physics_key, "elapsed_physics_time");
	EXPECT_EQ(clock_key, "elapsed_clock_time");
	EXPECT_EQ(frame_key, "physics_frame_count");
}

TEST_F(SnoreCoreTimeTest, TimeoutOperations) {
	// Test timeout creation and clearing.
	Callable callback = callable_mp(this, &SnoreCoreTimeTest::dummy_callback);
	int timeout_id = snore_core_time->set_timeout(callback, 1.0);

	EXPECT_GE(timeout_id, 0);

	bool cleared = snore_core_time->clear_timeout(timeout_id);
	EXPECT_EQ(cleared, true);

	bool cleared_again = snore_core_time->clear_timeout(timeout_id);
	EXPECT_EQ(cleared_again, false);
}

TEST_F(SnoreCoreTimeTest, IntervalOperations) {
	// Test interval creation and clearing.
	Callable callback = callable_mp(this, &SnoreCoreTimeTest::dummy_callback);
	int interval_id = snore_core_time->set_interval(callback, 1.0);

	EXPECT_GE(interval_id, 0);

	bool cleared = snore_core_time->clear_interval(interval_id);
	EXPECT_EQ(cleared, true);

	bool cleared_again = snore_core_time->clear_interval(interval_id);
	EXPECT_EQ(cleared_again, false);
}

TEST_F(SnoreCoreTimeTest, GarbageCollection) {
	// Test that garbage collection doesn't crash.
	snore_core_time->collect_garbage();
	// If we get here without crashing, the test passes
}

TEST_F(SnoreCoreTimeTest, ProcessHandling) {
	// Test that _process doesn't crash.
	snore_core_time->_process(0.016); // 60 FPS delta
	// If we get here without crashing, the test passes
}

} // namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_SNORE_CORE_TIME_H
