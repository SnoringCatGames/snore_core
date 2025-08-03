#ifndef TEST_log_service_H
#define TEST_log_service_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/log_service.h"

#include <gtest/gtest.h>

namespace godot {

TEST(LogServiceTest, PrintQueue) {
	LogService *log = memnew(LogService);
	log->set_up();

	log->print("Test message 1");
	log->print("Test message 2");

	EXPECT_EQ(log->get_recent_logs()->size(), 2);

	String first_message = log->get_recent_logs()->get(0);
	String second_message = log->get_recent_logs()->get(1);

	// Check that messages contain timestamps.
	EXPECT_TRUE(first_message.contains("Test message 1"));
	EXPECT_TRUE(second_message.contains("Test message 2"));
	EXPECT_TRUE(first_message.begins_with("["));
	EXPECT_TRUE(second_message.begins_with("["));

	log->reset();
	memdelete(log);
}

} // namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_log_service_H
