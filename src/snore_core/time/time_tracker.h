#ifndef TIME_TRACKER_H
#define TIME_TRACKER_H

#include "snore_core/time/time_service.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

class TimeTracker : public Node {
	GDCLASS(TimeTracker, Node)

public:
	TimeTracker() = default;
	~TimeTracker() = default;

	void _ready() override;
	void _process(double p_delta) override;
	void _physics_process(double p_delta) override;

	float get_time_scale() const { return time_scale; }
	void set_time_scale(float p_time_scale) { time_scale = p_time_scale; }

	float get_start_clock_time() const { return start_clock_time; }

	float get_elapsed_clock_time() const { return elapsed_clock_time; }
	float get_elapsed_physics_time() const { return elapsed_physics_time; }
	float get_elapsed_render_time() const { return elapsed_render_time; }

	float get_elapsed_clock_scaled_time() const {
		return elapsed_clock_scaled_time;
	}
	float get_elapsed_physics_scaled_time() const {
		return elapsed_physics_scaled_time;
	}
	float get_elapsed_render_scaled_time() const {
		return elapsed_render_scaled_time;
	}

	int get_physics_frame_count() const { return physics_frame_count; }
	int get_render_frame_count() const { return render_frame_count; }

protected:
	static void _bind_methods() {}

private:
	float time_scale = TimeService::default_time_scale;
	float start_clock_time = 0.0;
	float elapsed_clock_time = 0.0;
	float elapsed_physics_time = 0.0;
	float elapsed_render_time = 0.0;
	float elapsed_clock_scaled_time = 0.0;
	float elapsed_physics_scaled_time = 0.0;
	float elapsed_render_scaled_time = 0.0;
	int physics_frame_count = 0;
	int render_frame_count = 0;

	void _update_clock_time();
};

} // namespace godot

#endif // TIME_TRACKER_H
