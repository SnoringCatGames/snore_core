#ifndef TIME_TRACKER_H
#define TIME_TRACKER_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

// Keeps track of elapsed time.
class TimeTracker : public Node {
	GDCLASS(TimeTracker, Node)

private:
	float time_scale;
	float start_clock_time;
	float elapsed_clock_time;
	float elapsed_physics_time;
	float elapsed_render_time;
	float elapsed_clock_scaled_time;
	float elapsed_physics_scaled_time;
	float elapsed_render_scaled_time;
	int physics_frame_count;
	int render_frame_count;

	// Updates the clock time based on OS time.
	void _update_clock_time();

public:
	TimeTracker();
	~TimeTracker();

	// Initializes the time tracker.
	void _ready() override;
	// Updates render time tracking.
	void _process(double p_delta) override;
	// Updates physics time tracking.
	void _physics_process(double p_delta) override;

	// Gets the time scale multiplier.
	float get_time_scale() const { return time_scale; }
	// Sets the time scale multiplier.
	void set_time_scale(float p_time_scale) { time_scale = p_time_scale; }

	// Gets the clock start time.
	float get_start_clock_time() const { return start_clock_time; }

	// Gets the elapsed clock time.
	float get_elapsed_clock_time() const { return elapsed_clock_time; }
	// Gets the elapsed physics time.
	float get_elapsed_physics_time() const { return elapsed_physics_time; }
	// Gets the elapsed render time.
	float get_elapsed_render_time() const { return elapsed_render_time; }

	// Gets the elapsed scaled clock time.
	float get_elapsed_clock_scaled_time() const {
		return elapsed_clock_scaled_time;
	}
	// Gets the elapsed scaled physics time.
	float get_elapsed_physics_scaled_time() const {
		return elapsed_physics_scaled_time;
	}
	// Gets the elapsed scaled render time.
	float get_elapsed_render_scaled_time() const {
		return elapsed_render_scaled_time;
	}

	// Gets the physics frame count.
	int get_physics_frame_count() const { return physics_frame_count; }
	// Gets the render frame count.
	int get_render_frame_count() const { return render_frame_count; }

protected:
	static void _bind_methods();
};

} // namespace godot

#endif // TIME_TRACKER_H
