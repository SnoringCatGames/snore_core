#ifndef TEST_DEBOUNCER_H
#define TEST_DEBOUNCER_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/time/debouncer.h"

#include "snore_core/internal/test_utils.h"

#include <gtest/gtest.h>

namespace godot {

class DebouncerTest : public SnoreCoreTest {
protected:
	void BeforeEach() override { debouncer = memnew(Debouncer); }

	void AfterEach() override { memdelete(debouncer); }

	Debouncer *debouncer;
};

} // namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_DEBOUNCER_H
