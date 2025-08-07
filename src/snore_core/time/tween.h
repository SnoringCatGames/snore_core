#ifndef TWEEN_H
#define TWEEN_H

#include "snore_core/time/time_tracker.h"
#include "snore_core/time/time_type.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

// FIXME: LEFT OFF HERE: FINISH PORTING ---------------------------------------

class Tween : public RefCounted {
	GDCLASS(Tween, RefCounted)

public:
	Tween() = default;
	~Tween() = default;

protected:
	static void _bind_methods() {}

private:
};

} // namespace godot

#endif // TWEEN_H
