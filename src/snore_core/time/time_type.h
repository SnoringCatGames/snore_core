#ifndef TIME_TYPE_H
#define TIME_TYPE_H

#include "snore_core/internal/string_utils.h"

#include <godot_cpp/core/class_db.hpp>

namespace godot {

// The different types of time that are tracked.
//
// -   Fixed-interval/physics-based tracking vs clock and render-based
// tracking:
//     -   In general, use fixed-interval/physics-based tracking for most
//     cases,
//         unless you know you need something else.
//         -   Fixed-interval tracking is deterministic, which means you should
//             see the same results each time you run the app.
//     -   "Clock" and "render"-based tracking should represent actual time
//     more
//         accurately than fixed-interval/physics-based tracking, since the
//         latter under-counts if a frame actually takes longer than the
//         expected fixed-interval duration.
//         -   This is useful if you want to actual track and display real
//             elapsed time to the player.
// -   App time vs play time:
//     -   App time tracks total time elapsed while the app has been running.
//     -   Play time tracks elapsed **unpaused** time.
// -   Scaled vs un-scaled time:
//     -   Scaled time is modified by the combination of the `time_scale` and
//         `additional_debug_time_scale` properties.
//         -   The current scale is applied to each individual frame's delta
//         and
//             this is added to a running total.
//         -   So this accounts for changes in time scale.
enum TimeType {
	// -   Elapsed time from deterministic fixed-interval _physics_process
	// calls.
	// -   Total app run time; not affected by pausing.
	// -   Not affected by current time_scale or
	// additional_debug_time_scale.
	APP_PHYSICS,
	// -   Elapsed **real-world time**, according to the OS.
	// -   Total app run time; not affected by pausing.
	// -   Not affected by current time_scale or
	// additional_debug_time_scale.
	APP_CLOCK,
	// -   Elapsed time from deterministic fixed-interval _physics_process
	// calls.
	// -   Total app run time; not affected by pausing.
	// -   **Scaled** according to the current time_scale and
	//     additional_debug_time_scale.
	APP_PHYSICS_SCALED,
	// -   Elapsed **real-world time**, according to the OS.
	// -   Total app run time; not affected by pausing.
	// -   **Scaled** according to the current time_scale and
	//     additional_debug_time_scale.
	APP_CLOCK_SCALED,
	// -   The number of times _physics_process has been called.
	// -   Total app run time; not affected by pausing.
	// -   Not affected by current time_scale or
	// additional_debug_time_scale.
	APP_PHYSICS_FRAME_COUNT,
	// -   The number of times _process has been called.
	// -   Total app run time; not affected by pausing.
	// -   Not affected by current time_scale or
	// additional_debug_time_scale.
	APP_RENDER_FRAME_COUNT,

	// -   Elapsed time from deterministic fixed-interval _physics_process
	// calls.
	// -   Total **un-paused** run time.
	// -   Not affected by current time_scale or
	// additional_debug_time_scale.
	PLAY_PHYSICS,
	// -   Elapsed time from nondeterministic variable-interval _process
	// calls.
	// -   Total **un-paused** run time.
	// -   Not affected by current time_scale or
	// additional_debug_time_scale.
	PLAY_RENDER,
	// -   Elapsed time from deterministic fixed-interval _physics_process
	// calls.
	// -   Total **un-paused** run time.
	// -   **Scaled** according to the current time_scale and
	//     additional_debug_time_scale.
	PLAY_PHYSICS_SCALED,
	// -   Elapsed time from nondeterministic variable-interval _process
	// calls.
	// -   Total **un-paused** run time.
	// -   **Scaled** according to the current time_scale and
	//     additional_debug_time_scale.
	PLAY_RENDER_SCALED,
	// -   The number of times _physics_process has been called.
	// -   Total **un-paused** run time.
	// -   Not affected by current time_scale or
	// additional_debug_time_scale.
	PLAY_PHYSICS_FRAME_COUNT,
	// -   The number of times _process has been called.
	// -   Total **un-paused** run time.
	// -   Not affected by current time_scale or
	// additional_debug_time_scale.
	PLAY_RENDER_FRAME_COUNT,

	_TimeType_COUNT,
};

static constexpr const char *time_type_strings[TimeType::_TimeType_COUNT] = {
	"APP_PHYSICS",
	"APP_CLOCK",
	"APP_PHYSICS_SCALED",
	"APP_CLOCK_SCALED",
	"APP_PHYSICS_FRAME_COUNT",
	"APP_RENDER_FRAME_COUNT",
	"PLAY_PHYSICS",
	"PLAY_RENDER",
	"PLAY_PHYSICS_SCALED",
	"PLAY_RENDER_SCALED",
	"PLAY_PHYSICS_FRAME_COUNT",
	"PLAY_RENDER_FRAME_COUNT",
};

static String time_type_to_string(TimeType p_time_type) {
	return time_type_strings[p_time_type];
}

static String get_time_type_hint_string() {
	return join_strings(time_type_strings, TimeType::_TimeType_COUNT, ",");
}

} // namespace godot

VARIANT_ENUM_CAST(TimeType);

#endif // TIME_TYPE_H
