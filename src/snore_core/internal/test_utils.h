#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#ifdef SC_TESTS_ENABLED

#include <gtest/gtest.h>
#include <godot_cpp/classes/ref.hpp>

#include <algorithm>

namespace godot {

#define EXPECT_VECTOR2_EQ(m_expected, m_actual)                                \
	do {                                                                       \
		EXPECT_FLOAT_EQ(m_expected.x, m_actual.x);                             \
		EXPECT_FLOAT_EQ(m_expected.y, m_actual.y);                             \
	} while (0)

#define EXPECT_STRING_EQ(m_expected, m_actual)                                 \
	EXPECT_EQ(String(m_expected), String(m_actual))

namespace TestUtilsInternal {
extern std::vector<String> recent_ensures;

extern bool was_an_ensure_expected;
} //namespace TestUtilsInternal

void clear_recent_ensures() {
	TestUtilsInternal::recent_ensures.clear();
	TestUtilsInternal::was_an_ensure_expected = false;
}

#define EXPECT_ENSURE_WITH_SUBSTRING(m_message_substring)                      \
	do {                                                                       \
		TestUtilsInternal::was_an_ensure_expected = true;                      \
		EXPECT_TRUE(                                                           \
				std::any_of(                                                   \
						TestUtilsInternal::recent_ensures.begin(),             \
						TestUtilsInternal::recent_ensures.end(),               \
						[&](const String &p_message) {                         \
							return p_message.find(m_message_substring) != -1;  \
						}));                                                   \
	} while (0)

#define EXPECT_NO_ENSURES()                                                    \
	EXPECT_TRUE(TestUtilsInternal::recent_ensures.empty())

class SnoreCoreTest : public ::testing::Test {
protected:
	virtual void BeforeEach() {}
	virtual void AfterEach() {}

	// Subclasses should override BeforeEach instead of the standard SetUp.
	virtual void SetUp() override final { BeforeEach(); }

	// Subclasses should override AfterEach instead of the standard TearDown.
	virtual void TearDown() override final {
		if (!TestUtilsInternal::was_an_ensure_expected) {
			EXPECT_TRUE(TestUtilsInternal::recent_ensures.empty());
		}
		clear_recent_ensures();
		AfterEach();
	}
};

} //namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_UTILS_H
