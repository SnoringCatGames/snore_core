#ifndef CANVAS_LAYER_NAME_H
#define CANVAS_LAYER_NAME_H

#include <godot_cpp/variant/string_name.hpp>

#include <vector>

#define CANVAS_LAYER_NAME(m_name)                                              \
	namespace Internal {                                                       \
	static const constexpr char *m_name##_canvas_layer_name = #m_name;         \
	} /*namespace Internal*/                                                   \
                                                                               \
	_FORCE_INLINE_ const StringName &m_name() {                                \
		static const StringName string_name =                                  \
				StringName(Internal::m_name##_canvas_layer_name);              \
		return string_name;                                                    \
	}

namespace godot {
namespace CanvasLayerName {

CANVAS_LAYER_NAME(utils)
CANVAS_LAYER_NAME(top)
CANVAS_LAYER_NAME(notifications)
CANVAS_LAYER_NAME(super_hud)
CANVAS_LAYER_NAME(screens)
CANVAS_LAYER_NAME(hud)
CANVAS_LAYER_NAME(annotations)
CANVAS_LAYER_NAME(game)

} //namespace CanvasLayerName
} //namespace godot

#endif // CANVAS_LAYER_NAME_H
