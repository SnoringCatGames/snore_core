#ifndef TEST_SNORE_CORE_UTILS_H
#define TEST_SNORE_CORE_UTILS_H

#ifdef SC_TESTS_ENABLED

#include "snore_core/snore_core_utils.h"

#include "snore_core/geometry_constants.h"
#include "snore_core/internal/debug_utils.h"
#include "snore_core/internal/test_utils.h"

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_state.hpp>
#include <godot_cpp/classes/scroll_container.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <gtest/gtest.h>

namespace godot {

class SnoreCoreUtilsTest : public SnoreCoreTest {
protected:
	void BeforeEach() override { utils = memnew(SnoreCoreUtils); }

	void AfterEach() override { memdelete(utils); }

	SnoreCoreUtils *utils;
};

TEST_F(SnoreCoreUtilsTest, Ensure) {
	// Test ensure with true condition.
	bool result = utils->ensure(true, "Test message");
	EXPECT_EQ(result, true);
	EXPECT_NO_ENSURES();

	// Test ensure with false condition.
	DISABLE_ENSURE_BREAKPOINTS();
	result = utils->ensure(false, "Test failure message");
	EXPECT_EQ(result, false);
	EXPECT_ENSURE_WITH_SUBSTRING("Test failure message");
}

TEST_F(SnoreCoreUtilsTest, Splice) {
	// Test inserting items in the middle.
	Array result = { "a", "b", "c", "d" };
	Array items_to_insert = { "e", "f" };
	SnoreCoreUtils::splice(result, 2, 0, items_to_insert);
	EXPECT_EQ(result, Array({ "a", "b", "e", "f", "c", "d" }));

	// Test deleting and inserting items.
	result = { "a", "b", "c", "d" };
	items_to_insert = { "e", "f" };
	SnoreCoreUtils::splice(result, 2, 2, items_to_insert);
	EXPECT_EQ(result, Array({ "a", "b", "e", "f" }));

	// Test only deleting items.
	result = { "a", "b", "c", "d" };
	items_to_insert = {};
	SnoreCoreUtils::splice(result, 0, 2, items_to_insert);
	EXPECT_EQ(result, Array({ "c", "d" }));
}

TEST_F(SnoreCoreUtilsTest, Dedup) {
	const Array input = { "a", "b", "a", "c", "b" };
	const Array result = SnoreCoreUtils::dedup(input);
	EXPECT_EQ(result, Array({ "a", "b", "c" }));
}

TEST_F(SnoreCoreUtilsTest, ArrayToSet) {
	const Array input = { "a", "b", "c" };
	const Dictionary result = SnoreCoreUtils::array_to_set(input);
	Dictionary expected;
	expected["a"] = true;
	expected["b"] = true;
	expected["c"] = true;
	EXPECT_EQ(result, expected);
}

TEST_F(SnoreCoreUtilsTest, SubtractNestedArrays) {
	// Test basic array subtraction.
	Dictionary result;
	result["items"] = Array({ "a", "b", "c" });
	Dictionary other;
	other["items"] = Array({ "b" });
	utils->subtract_nested_arrays(result, other, false);
	EXPECT_EQ(Array(result["items"]), Array({ "a", "c" }));

	// Test nested dictionary subtraction.
	Dictionary result_nested;
	Dictionary inner_result;
	inner_result["numbers"] = Array({ 1, 2, 3 });
	result_nested["nested"] = inner_result;
	Dictionary other_nested;
	Dictionary inner_other;
	inner_other["numbers"] = Array({ 2 });
	other_nested["nested"] = inner_other;
	utils->subtract_nested_arrays(result_nested, other_nested, false);
	Dictionary expected_inner = result_nested["nested"];
	EXPECT_EQ(expected_inner["numbers"], Array({ 1, 3 }));

	// Test missing key handling with expects_no_missing_matches = false.
	Dictionary result_missing;
	result_missing["items"] = Array({ "x" });
	Dictionary other_missing;
	other_missing["different_key"] = Array({ "y" });
	// This should not cause an error since expects_no_missing_matches = false.
	utils->subtract_nested_arrays(result_missing, other_missing, false);
	// The result should remain unchanged since no matching key was found.
	EXPECT_EQ(result_missing["items"], Array({ "x" }));

	// Test array element not found with expects_no_missing_matches = false.
	Dictionary result_not_found;
	result_not_found["items"] = Array({ "a" });
	Dictionary other_not_found;
	other_not_found["items"] = Array({ "z" }); // Element not in result array.
	// This should not cause an error since expects_no_missing_matches = false.
	utils->subtract_nested_arrays(result_not_found, other_not_found, false);
	// The result should remain unchanged since no matching element was found.
	EXPECT_EQ(result_not_found["items"], Array({ "a" }));
}

TEST_F(SnoreCoreUtilsTest, TranslatePolyline) {
	const PackedVector2Array vertices = {
		Vector2(0, 0),
		Vector2(1, 1),
		Vector2(2, 2),
	};
	const Vector2 translation(10, 20);
	const PackedVector2Array result =
			SnoreCoreUtils::translate_polyline(vertices, translation);
	const PackedVector2Array expected = {
		Vector2(10, 20),
		Vector2(11, 21),
		Vector2(12, 22),
	};
	EXPECT_EQ(result, expected);
}

TEST_F(SnoreCoreUtilsTest, IsNum) {
	EXPECT_TRUE(SnoreCoreUtils::is_num(42));
	EXPECT_TRUE(SnoreCoreUtils::is_num(3.14));
	EXPECT_FALSE(SnoreCoreUtils::is_num("text"));
	EXPECT_FALSE(SnoreCoreUtils::is_num(Vector2(1, 2)));
}

TEST_F(SnoreCoreUtilsTest, FloorVector) {
	const Vector2 input(3.7, -2.3);
	const Vector2 result = SnoreCoreUtils::floor_vector(input);
	EXPECT_EQ(result, Vector2(3.0f, -3.0f));
}

TEST_F(SnoreCoreUtilsTest, CeilVector) {
	const Vector2 input(3.2, -2.7);
	const Vector2 result = SnoreCoreUtils::ceil_vector(input);
	EXPECT_EQ(result, Vector2(4.0f, -2.0f));
}

TEST_F(SnoreCoreUtilsTest, RoundVector) {
	const Vector2 input(3.6, -2.4);
	const Vector2 result = SnoreCoreUtils::round_vector(input);
	EXPECT_EQ(result, Vector2(4.0f, -2.0f));
}

TEST_F(SnoreCoreUtilsTest, GetTypeString) {
	EXPECT_STRING_EQ(SnoreCoreUtils::get_type_string(Variant::INT), "TYPE_INT");
	EXPECT_STRING_EQ(
			SnoreCoreUtils::get_type_string(Variant::FLOAT), "TYPE_FLOAT");
	EXPECT_STRING_EQ(
			SnoreCoreUtils::get_type_string(Variant::STRING), "TYPE_STRING");
	EXPECT_STRING_EQ(
			SnoreCoreUtils::get_type_string(Variant::VECTOR2), "TYPE_VECTOR2");
}

TEST_F(SnoreCoreUtilsTest, GetSpaces) {
	String result = utils->get_spaces(0);
	EXPECT_STRING_EQ(result, "");

	result = utils->get_spaces(5);
	EXPECT_STRING_EQ(result, "     ");
	EXPECT_EQ(result.length(), 5);
}

TEST_F(SnoreCoreUtilsTest, PadString) {
	String result = utils->pad_string("test", 8, true, false);
	EXPECT_STRING_EQ(result, "test    ");
	EXPECT_EQ(result.length(), 8);

	result = utils->pad_string("test", 8, false, false);
	EXPECT_STRING_EQ(result, "    test");
	EXPECT_EQ(result.length(), 8);
}

TEST_F(SnoreCoreUtilsTest, ResizeString) {
	String result = utils->resize_string("test", 8, true);
	EXPECT_STRING_EQ(result, "test    ");

	result = utils->resize_string("verylongstring", 5, true);
	EXPECT_STRING_EQ(result, "veryl");
	EXPECT_EQ(result.length(), 5);

	result = utils->resize_string("exact", 5, true);
	EXPECT_STRING_EQ(result, "exact");
}

TEST_F(SnoreCoreUtilsTest, GetVectorString) {
	const Vector2 input(3.14159, 2.71828);
	const String result = utils->get_vector_string(input, 2);
	EXPECT_STRING_EQ(result, "(3.14,2.72)");
}

TEST_F(SnoreCoreUtilsTest, Mix) {
	// Test mixing numbers.
	const Array values = { 10.0, 20.0, 30.0 };
	const TypedArray<float> weights = { 0.5, 0.3, 0.2 };
	const Variant result = utils->mix(values, weights);
	// Expected: (10.0 * 0.5 + 20.0 * 0.3 + 30.0 * 0.2) = 5.0 + 6.0 + 6.0
	// = 17.0.
	EXPECT_FLOAT_EQ(float(result), 17.0);

	// Test mixing Vector2 values.
	const Array vector_values = { Vector2(1.0, 2.0), Vector2(3.0, 4.0) };
	const TypedArray<float> vector_weights = { 0.6, 0.4 };
	const Variant vector_result = utils->mix(vector_values, vector_weights);
	const Vector2 expected_vector =
			Vector2(1.0, 2.0) * 0.6 + Vector2(3.0, 4.0) * 0.4;
	EXPECT_EQ(Vector2(vector_result), expected_vector);

	// Test with equal weights (should normalize to 1/count each).
	const TypedArray<float> equal_weights = { 1.0, 1.0 };
	const Variant equal_result = utils->mix(vector_values, equal_weights);
	const Vector2 expected_equal =
			(Vector2(1.0, 2.0) + Vector2(3.0, 4.0)) * 0.5;
	EXPECT_EQ(Vector2(equal_result), expected_equal);
}

TEST_F(SnoreCoreUtilsTest, MixColors) {
	const Color red(1.0, 0.0, 0.0, 1.0);
	const Color green(0.0, 1.0, 0.0, 1.0);
	const Color blue(0.0, 0.0, 1.0, 1.0);
	const Array colors = { red, green, blue };
	const TypedArray<float> weights = { 0.5, 0.3, 0.2 };
	const Color result = utils->mix_colors(colors, weights);

	// Calculate expected HSV values.
	const float expected_h =
			red.get_h() * 0.5 + green.get_h() * 0.3 + blue.get_h() * 0.2;
	const float expected_s =
			red.get_s() * 0.5 + green.get_s() * 0.3 + blue.get_s() * 0.2;
	const float expected_v =
			red.get_v() * 0.5 + green.get_v() * 0.3 + blue.get_v() * 0.2;
	const Color expected =
			Color::from_hsv(expected_h, expected_s, expected_v, 1.0);

	EXPECT_FLOAT_EQ(result.get_h(), expected.get_h());
	EXPECT_FLOAT_EQ(result.get_s(), expected.get_s());
	EXPECT_FLOAT_EQ(result.get_v(), expected.get_v());
	EXPECT_FLOAT_EQ(result.a, 1.0);
}

TEST_F(SnoreCoreUtilsTest, GetDatetimeString) {
	const String result = utils->get_datetime_string();

	// Test that the result follows the expected format pattern.
	// Format should be: YYYY-MM-DD_HH:MM:SS.MMM.
	EXPECT_TRUE(result.length() >= 23);

	// Check basic structure (contains expected separators).
	EXPECT_TRUE(result.contains("-"));
	EXPECT_TRUE(result.contains("_"));
	EXPECT_TRUE(result.contains(":"));
	EXPECT_TRUE(result.contains("."));

	// Verify it's not empty.
	EXPECT_FALSE(result.is_empty());
}

TEST_F(SnoreCoreUtilsTest, GetTimeString) {
	const String result = utils->get_time_string();

	// Test that the result follows the expected format pattern.
	// Format should be: HH:MM:SS.MMM.
	EXPECT_EQ(result.length(), 12);

	// Check basic structure.
	EXPECT_TRUE(result.contains(":"));
	EXPECT_TRUE(result.contains("."));

	// Verify it's not empty.
	EXPECT_FALSE(result.is_empty());
}

TEST_F(SnoreCoreUtilsTest, GetTimeStringFromSeconds) {
	// Test basic seconds formatting.
	String result =
			utils->get_time_string_from_seconds(65.0, false, false, false);
	EXPECT_STRING_EQ(result, "01:05");

	// Test with milliseconds.
	result = utils->get_time_string_from_seconds(65.123, true, false, false);
	EXPECT_STRING_EQ(result, "01:05.123");

	// Test with hours.
	result = utils->get_time_string_from_seconds(3661.0, false, true, true);
	EXPECT_STRING_EQ(result, "01:01:01");

	// Test with empty hours and minutes.
	result = utils->get_time_string_from_seconds(5.0, false, true, true);
	EXPECT_STRING_EQ(result, "00:00:05");

	// Test undefined time (inf).
	result = utils->get_time_string_from_seconds(inf, false, true, true);
	EXPECT_STRING_EQ(result, "--:--:--");

	// Test undefined time with milliseconds.
	result = utils->get_time_string_from_seconds(inf, true, true, true);
	EXPECT_STRING_EQ(result, "--:--:--.---");

	// Test zero seconds.
	result = utils->get_time_string_from_seconds(0.0, false, false, false);
	EXPECT_STRING_EQ(result, "00");

	// Test large values.
	result = utils->get_time_string_from_seconds(7265.456, true, true, true);
	EXPECT_STRING_EQ(result, "02:01:05.456");
}

} //namespace godot

#endif // SC_TESTS_ENABLED

#endif // TEST_SNORE_CORE_UTILS_H
