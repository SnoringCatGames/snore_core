#ifndef TEST_TWEEN_H
#define TEST_TWEEN_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/time/tween.h"

#include "snore_core/internal/test_utils.h"

#include <gtest/gtest.h>
#include <godot_cpp/classes/node.hpp>

namespace godot {

class TweenTest : public SnoreCoreTest {
protected:
	void BeforeEach() override {
		parent_node = memnew(Node);
		tween = memnew(SnoreCoreTween);
		tween->_init(parent_node, true);
		target_object = memnew(Node);
	}

	void AfterEach() override {
		if (target_object) {
			memdelete(target_object);
		}
		if (tween) {
			memdelete(tween);
		}
		if (parent_node) {
			memdelete(parent_node);
		}
	}

	Node *parent_node;
	SnoreCoreTween *tween;
	Node *target_object;

private:
	void dummy_callback() {}
};

TEST_F(TweenTest, InterpolateProperty) {
	// Test interpolate_property creates a pending sub-tween.
	Vector2 initial_pos(0, 0);
	Vector2 final_pos(100, 100);

	tween->interpolate_property(
			target_object, NodePath("position"), initial_pos, final_pos, 1.0f,
			EaseType::EASE_IN_OUT, 0.0f);

	// Tween should still be inactive until started.
	EXPECT_FALSE(tween->is_active());

	// Start the tween.
	EXPECT_TRUE(tween->start());
	EXPECT_TRUE(tween->is_active());
}

TEST_F(TweenTest, InterpolateMethod) {
	// Test interpolate_method creates a pending sub-tween.
	float initial_value = 0.0f;
	float final_value = 100.0f;

	tween->interpolate_method(
			target_object, "set_scale", initial_value, final_value, 1.0f,
			EaseType::LINEAR, 0.0f);

	// Tween should still be inactive until started.
	EXPECT_FALSE(tween->is_active());

	// Start the tween.
	EXPECT_TRUE(tween->start());
	EXPECT_TRUE(tween->is_active());
}

TEST_F(TweenTest, StartWithNoPendingTweens) {
	// Test that starting a tween with no pending sub-tweens returns false.
	EXPECT_FALSE(tween->start());
	EXPECT_FALSE(tween->is_active());
}

TEST_F(TweenTest, StopAll) {
	// Test stop_all functionality.
	tween->interpolate_property(
			target_object, NodePath("position"), Vector2(0, 0),
			Vector2(100, 100), 1.0f);

	tween->start();
	EXPECT_TRUE(tween->is_active());

	EXPECT_TRUE(tween->stop_all());
	EXPECT_FALSE(tween->is_active());

	// Calling stop_all when no tweens are active should return false.
	EXPECT_FALSE(tween->stop_all());
}

} // namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_TWEEN_H
