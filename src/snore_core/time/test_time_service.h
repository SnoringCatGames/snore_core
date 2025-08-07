#ifndef TEST_TIME_SERVICE_H
#define TEST_TIME_SERVICE_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/time/time_service.h"

#include "snore_core/internal/test_utils.h"
#include "snore_core/time/time_type.h"

#include <gtest/gtest.h>

namespace godot {

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

class TimeServiceTest : public SnoreCoreTest {
protected:
	void BeforeEach() override {
		time_service = memnew(TimeService);
		time_service->_init();
		time_service->_ready();
	}

	void AfterEach() override { memdelete(time_service); }

	TimeService *time_service;
};

TEST_F(TimeServiceTest, Constants) {
	// Test that constants are properly defined.
	EXPECT_EQ(TimeService::physics_fps, 60.0);
	EXPECT_EQ(TimeService::physics_time_step, 1.0 / 60.0);
}

TEST_F(TimeServiceTest, InitialState) {
	// Test that a new scaffolder time has expected initial values.
	EXPECT_EQ(time_service->get_time_scale(), 1.0);
	EXPECT_EQ(time_service->get_additional_debug_time_scale(), 1.0);
	EXPECT_EQ(time_service->get_combined_scale(), 1.0);
}

TEST_F(TimeServiceTest, TimeScaleSetting) {
	// Test setting time scales.
	time_service->set_time_scale(2.0);
	EXPECT_EQ(time_service->get_time_scale(), 2.0);
	EXPECT_EQ(time_service->get_combined_scale(), 2.0);

	time_service->set_additional_debug_time_scale(0.5);
	EXPECT_EQ(time_service->get_additional_debug_time_scale(), 0.5);
	EXPECT_EQ(time_service->get_combined_scale(), 1.0);
}

TEST_F(TimeServiceTest, ScalingFunctions) {
	// Test time scaling functions.
	time_service->set_time_scale(2.0);

	float duration = 1.0;
	float scaled_duration = time_service->scale_delta(duration);
	EXPECT_EQ(scaled_duration, 2.0);

	float scaled_time_step = time_service->get_scaled_time_step();
	float expected = TimeService::physics_time_step * 2.0;
	EXPECT_EQ(scaled_time_step, expected);
}

TEST_F(TimeServiceTest, TaskIdGeneration) {
	// Test that task IDs are generated incrementally.
	int id1 = time_service->get_next_task_id();
	int id2 = time_service->get_next_task_id();
	EXPECT_EQ(id2, id1 + 1);
}

TEST_F(TimeServiceTest, TimeTrackerAccess) {
	// Test accessing time trackers for different time types.
	TimeTracker *app_tracker =
			time_service->get_time_tracker_for_time_type(TimeType::APP_PHYSICS);
	TimeTracker *play_tracker = time_service->get_time_tracker_for_time_type(
			TimeType::PLAY_PHYSICS);

	EXPECT_NE(app_tracker, nullptr);
	EXPECT_NE(play_tracker, nullptr);
	EXPECT_NE(app_tracker, play_tracker);
}

TEST_F(TimeServiceTest, ElapsedTimeKeys) {
	// Test that elapsed time keys are returned correctly.
	StringName physics_key = time_service->get_elapsed_time_type_for_time_type(
			TimeType::APP_PHYSICS);
	StringName clock_key = time_service->get_elapsed_time_type_for_time_type(
			TimeType::APP_CLOCK);
	StringName frame_key = time_service->get_elapsed_time_type_for_time_type(
			TimeType::APP_PHYSICS_FRAME_COUNT);

	EXPECT_EQ(physics_key, "elapsed_physics_time");
	EXPECT_EQ(clock_key, "elapsed_clock_time");
	EXPECT_EQ(frame_key, "physics_frame_count");
}

TEST_F(TimeServiceTest, TimeoutOperations) {
	// Test timeout creation and clearing.
	Callable callback = callable_mp(this, &TimeServiceTest::dummy_callback);
	int timeout_id = time_service->set_timeout(callback, 1.0);

	EXPECT_GE(timeout_id, 0);

	bool cleared = time_service->clear_timeout(timeout_id);
	EXPECT_EQ(cleared, true);

	bool cleared_again = time_service->clear_timeout(timeout_id);
	EXPECT_EQ(cleared_again, false);
}

TEST_F(TimeServiceTest, IntervalOperations) {
	// Test interval creation and clearing.
	Callable callback = callable_mp(this, &TimeServiceTest::dummy_callback);
	int interval_id = time_service->set_interval(callback, 1.0);

	EXPECT_GE(interval_id, 0);

	bool cleared = time_service->clear_interval(interval_id);
	EXPECT_EQ(cleared, true);

	bool cleared_again = time_service->clear_interval(interval_id);
	EXPECT_EQ(cleared_again, false);
}

TEST_F(TimeServiceTest, GarbageCollection) {
	// Test that garbage collection doesn't crash.
	time_service->collect_garbage();
	// If we get here without crashing, the test passes
}

TEST_F(TimeServiceTest, ProcessHandling) {
	// Test that _process doesn't crash.
	time_service->_process(0.016); // 60 FPS delta
	// If we get here without crashing, the test passes
}

} // namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_TIME_SERVICE_H
