#ifndef TEST_CIRCULAR_BUFFER_H
#define TEST_CIRCULAR_BUFFER_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/circular_buffer.h"

#include "snore_core/internal/test_utils.h"

#include <gtest/gtest.h>

namespace godot {

class CircularBufferTest : public SnoreCoreTest {
protected:
	void BeforeEach() override { buffer.instantiate(); }

	void AfterEach() override { buffer.unref(); }

	Ref<CircularBuffer> buffer;
};

TEST_F(CircularBufferTest, PushAndGet) {
	buffer->initialize(3);

	buffer->push("first");
	buffer->push("second");

	EXPECT_EQ(buffer->get(0), "first");
	EXPECT_EQ(buffer->get(1), "second");
	EXPECT_EQ(buffer->size(), 2);
}

TEST_F(CircularBufferTest, WrapAround) {
	buffer->initialize(3);

	buffer->push("first");
	buffer->push("second");
	buffer->push("third");
	// Should wrap around.
	buffer->push("fourth");

	EXPECT_EQ(buffer->size(), 3);
	EXPECT_EQ(buffer->get(0), "second");
	EXPECT_EQ(buffer->get(1), "third");
	EXPECT_EQ(buffer->get(2), "fourth");
}

TEST_F(CircularBufferTest, Clear) {
	buffer->initialize(3);

	buffer->push("first");
	buffer->push("second");
	buffer->clear();

	EXPECT_EQ(buffer->size(), 0);
}

TEST_F(CircularBufferTest, EmptyBuffer) {
	buffer->initialize(3);

	EXPECT_EQ(buffer->size(), 0);
	// Should return empty variant.
	EXPECT_EQ(buffer->get(0), Variant());
}

TEST_F(CircularBufferTest, FullBuffer) {
	buffer->initialize(2);

	buffer->push("first");
	buffer->push("second");
	// Should overwrite first.
	buffer->push("third");

	EXPECT_EQ(buffer->size(), 2);
	EXPECT_EQ(buffer->get(0), "second");
	EXPECT_EQ(buffer->get(1), "third");
}

} // namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_CIRCULAR_BUFFER_H
