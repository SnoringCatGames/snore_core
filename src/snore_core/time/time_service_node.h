#ifndef TIME_SERVICE_NODE_H
#define TIME_SERVICE_NODE_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

class TimeServiceNode : public Node {
	GDCLASS(TimeServiceNode, Node)

public:
	TimeServiceNode() = default;
	~TimeServiceNode() = default;

	void _ready() override;
	void _process(double p_delta) override;

protected:
	static void _bind_methods() {}
};

} // namespace godot

#endif // TIME_SERVICE_NODE_H
