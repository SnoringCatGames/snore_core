#ifndef TEST_TIME_TRACKER_H
#define TEST_TIME_TRACKER_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/time/time_tracker.h"

#include "snore_core/internal/test_utils.h"

#include <gtest/gtest.h>

namespace godot {

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

class TimeTrackerTest : public SnoreCoreTest {
protected:
	void BeforeEach() override { time_tracker = memnew(TimeTracker); }

	void AfterEach() override { memdelete(time_tracker); }

	TimeTracker *time_tracker;
};

TEST_F(TimeTrackerTest, InitialState) {
	// Test that a new time tracker has expected initial values.
	EXPECT_EQ(time_tracker->get_time_scale(), 1.0);
	EXPECT_EQ(time_tracker->get_elapsed_clock_time(), 0.0);
	EXPECT_EQ(time_tracker->get_elapsed_physics_time(), 0.0);
	EXPECT_EQ(time_tracker->get_elapsed_render_time(), 0.0);
	EXPECT_EQ(time_tracker->get_elapsed_clock_scaled_time(), 0.0);
	EXPECT_EQ(time_tracker->get_elapsed_physics_scaled_time(), 0.0);
	EXPECT_EQ(time_tracker->get_elapsed_render_scaled_time(), 0.0);
	EXPECT_EQ(time_tracker->get_physics_frame_count(), 0);
	EXPECT_EQ(time_tracker->get_render_frame_count(), 0);
}

TEST_F(TimeTrackerTest, TimeScaleSetting) {
	// Test setting and getting time scale.
	float test_scale = 2.5;
	time_tracker->set_time_scale(test_scale);
	EXPECT_EQ(time_tracker->get_time_scale(), test_scale);
}

TEST_F(TimeTrackerTest, ReadyInitialization) {
	// Test that _ready() initializes start time.
	time_tracker->_ready();
	EXPECT_GT(time_tracker->get_start_clock_time(), 0.0);
}

TEST_F(TimeTrackerTest, ProcessCallIncrements) {
	// Test that _process() increments render frame count and time.
	time_tracker->_ready();

	float delta = 0.016; // 60 FPS
	time_tracker->_process(delta);

	EXPECT_EQ(time_tracker->get_render_frame_count(), 1);
	EXPECT_EQ(time_tracker->get_elapsed_render_time(), delta);
	EXPECT_EQ(time_tracker->get_elapsed_render_scaled_time(), delta);
}

TEST_F(TimeTrackerTest, PhysicsProcessIncrements) {
	// Test that _physics_process() increments physics frame count and time.
	time_tracker->_ready();

	float delta = 0.016; // 60 FPS
	time_tracker->_physics_process(delta);

	EXPECT_EQ(time_tracker->get_physics_frame_count(), 1);
	EXPECT_EQ(time_tracker->get_elapsed_physics_time(), delta);
	EXPECT_EQ(time_tracker->get_elapsed_physics_scaled_time(), delta);
}

TEST_F(TimeTrackerTest, ScaledTimeCalculation) {
	// Test that scaled time is calculated correctly with custom time scale.
	time_tracker->_ready();
	time_tracker->set_time_scale(2.0);

	float delta = 0.016;
	time_tracker->_process(delta);
	time_tracker->_physics_process(delta);

	EXPECT_EQ(time_tracker->get_elapsed_render_scaled_time(), delta * 2.0);
	EXPECT_EQ(time_tracker->get_elapsed_physics_scaled_time(), delta * 2.0);
}

TEST_F(TimeTrackerTest, MultipleFrameAccumulation) {
	// Test that time accumulates correctly over multiple frames.
	time_tracker->_ready();

	float delta = 0.016;
	time_tracker->_process(delta);
	time_tracker->_process(delta);
	time_tracker->_physics_process(delta);

	EXPECT_EQ(time_tracker->get_render_frame_count(), 2);
	EXPECT_EQ(time_tracker->get_physics_frame_count(), 1);
	EXPECT_EQ(time_tracker->get_elapsed_render_time(), delta * 2.0);
	EXPECT_EQ(time_tracker->get_elapsed_physics_time(), delta);
}

} // namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_TIME_TRACKER_H
