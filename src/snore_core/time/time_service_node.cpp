#include "snore_core/time/time_service_node.h"

#include "snore_core/time/time_service.h"

using namespace godot;

void TimeServiceNode::_ready() { set_process_mode(PROCESS_MODE_ALWAYS); }

void TimeServiceNode::_process(double p_delta) {
	TimeService::get()->handle_frame(p_delta);
}
