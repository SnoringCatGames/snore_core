#ifndef SNORE_CORE_SETTINGS_H
#define SNORE_CORE_SETTINGS_H

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/binder_common.hpp>

namespace godot {

#define SC_SETTINGS_CLASS_DECLARATION(m_class)                                 \
public:                                                                        \
	static m_class *get();                                                     \
	static m_class *get_maybe();

#define SC_SETTINGS_CLASS_DEFINITION(m_class, m_parent_class)                  \
	static m_class *m_class::get() {                                           \
		m_parent_class *parent = m_parent_class::get();                        \
		if (!ENSURE(parent, #m_parent_class " is not initialized.")) {         \
			return nullptr;                                                    \
		}                                                                      \
		m_class *settings = parent->get_settings();                            \
		if (!ENSURE(settings, #m_class " is not initialized.")) {              \
			return nullptr;                                                    \
		}                                                                      \
		return settings;                                                       \
	}                                                                          \
                                                                               \
	static m_class *m_class::get_maybe() {                                     \
		m_parent_class *parent = m_parent_class::get();                        \
		if (parent) {                                                          \
			m_class *settings = parent->get_settings();                        \
			if (settings) {                                                    \
				return settings;                                               \
			}                                                                  \
		}                                                                      \
		return nullptr;                                                        \
	}

class SnoreCoreSettings : public Resource {
	GDCLASS(SnoreCoreSettings, Resource)

public:
	SnoreCoreSettings() = default;
	virtual ~SnoreCoreSettings() = default;

protected:
	static void _bind_methods() {}
};

} // namespace godot

#endif // SNORE_CORE_SETTINGS_H
