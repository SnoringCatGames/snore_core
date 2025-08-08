#ifndef TEST_TIME_SERVICE_H
#define TEST_TIME_SERVICE_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/time/time_service.h"

#include "snore_core/internal/test_utils.h"
#include "snore_core/time/time_type.h"

#include <gtest/gtest.h>
#include <godot_cpp/variant/callable.hpp>

namespace godot {

class TimeServiceTest : public SnoreCoreTest {
protected:
	void BeforeEach() override { time_service = memnew(TimeService); }

	void AfterEach() override { memdelete(time_service); }

	TimeService *time_service;
};

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

	const float duration = 1.0;
	const float scaled_duration = time_service->scale_delta(duration);
	EXPECT_EQ(scaled_duration, 2.0);

	const float scaled_time_step = time_service->get_scaled_time_step();
	const float expected = TimeService::physics_time_step * 2.0;
	EXPECT_EQ(scaled_time_step, expected);
}

TEST_F(TimeServiceTest, TaskIdGeneration) {
	// Test that task IDs are generated incrementally.
	const int id1 = time_service->get_next_task_id();
	const int id2 = time_service->get_next_task_id();
	EXPECT_EQ(id2, id1 + 1);
}

TEST_F(TimeServiceTest, TimeTrackerAccess) {
	// Test accessing time trackers for different time types.
	const TimeTracker *app_tracker =
			time_service->get_time_tracker_for_time_type(TimeType::APP_PHYSICS);
	const TimeTracker *play_tracker =
			time_service->get_time_tracker_for_time_type(
					TimeType::PLAY_PHYSICS);

	EXPECT_NE(app_tracker, nullptr);
	EXPECT_NE(play_tracker, nullptr);
	EXPECT_NE(app_tracker, play_tracker);
}

TEST_F(TimeServiceTest, ElapsedTimeKeys) {
	// Test that elapsed time keys are returned correctly.
	const ElapsedTimeType physics_type =
			time_service->get_elapsed_time_type_for_time_type(
					TimeType::APP_PHYSICS);
	const ElapsedTimeType clock_type =
			time_service->get_elapsed_time_type_for_time_type(
					TimeType::APP_CLOCK);
	const ElapsedTimeType frame_type =
			time_service->get_elapsed_time_type_for_time_type(
					TimeType::APP_PHYSICS_FRAME_COUNT);

	EXPECT_EQ(physics_type, ElapsedTimeType::ELAPSED_PHYSICS_TIME);
	EXPECT_EQ(clock_type, ElapsedTimeType::ELAPSED_CLOCK_TIME);
	EXPECT_EQ(frame_type, ElapsedTimeType::PHYSICS_FRAME_COUNT);
}

TEST_F(TimeServiceTest, TimeoutOperations) {
	// Test timeout creation and clearing.
	const Callable callback;
	const int timeout_id = time_service->set_timeout(callback, 1.0);

	EXPECT_GE(timeout_id, 0);

	const bool cleared = time_service->clear_timeout(timeout_id);
	EXPECT_EQ(cleared, true);

	const bool cleared_again = time_service->clear_timeout(timeout_id);
	EXPECT_EQ(cleared_again, false);
}

TEST_F(TimeServiceTest, IntervalOperations) {
	// Test interval creation and clearing.
	const Callable callback;
	const int interval_id = time_service->set_interval(callback, 1.0);

	EXPECT_GE(interval_id, 0);

	const bool cleared = time_service->clear_interval(interval_id);
	EXPECT_EQ(cleared, true);

	const bool cleared_again = time_service->clear_interval(interval_id);
	EXPECT_EQ(cleared_again, false);
}

} // namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_TIME_SERVICE_H
