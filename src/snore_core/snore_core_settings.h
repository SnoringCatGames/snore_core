#ifndef SNORE_CORE_SETTINGS_H
#define SNORE_CORE_SETTINGS_H

#include "snore_core/internal/ref_utils.h"

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/binder_common.hpp>

namespace godot {

#define SC_SETTINGS_CLASS_DECLARATION(m_class)                                 \
public:                                                                        \
	static Ref<m_class> get();                                                 \
	static Ref<m_class> get_maybe();

#define SC_SETTINGS_CLASS_DEFINITION_ON_MODULE(m_class, m_parent_class)        \
	Ref<m_class> m_class::get() {                                              \
		m_parent_class *parent = m_parent_class::get();                        \
		if (!ENSURE(parent, #m_parent_class " is not initialized.")) {         \
			return nullptr;                                                    \
		}                                                                      \
		Ref<m_class> settings = parent->get_settings();                        \
		if (!ENSURE(is_valid(settings), #m_class " is not initialized.")) {    \
			return nullptr;                                                    \
		}                                                                      \
		return settings;                                                       \
	}                                                                          \
                                                                               \
	Ref<m_class> m_class::get_maybe() {                                        \
		m_parent_class *parent = m_parent_class::get();                        \
		if (parent) {                                                          \
			Ref<m_class> settings = parent->get_settings();                    \
			if (is_valid(settings)) {                                          \
				return settings;                                               \
			}                                                                  \
		}                                                                      \
		return nullptr;                                                        \
	}

#define SC_SETTINGS_CLASS_DEFINITION_ON_SETTINGS(                              \
		m_class, m_parent_class, m_getter)                                     \
	Ref<m_class> m_class::get() {                                              \
		Ref<m_parent_class> parent = m_parent_class::get();                    \
		if (!ENSURE(is_valid(parent),                                          \
					#m_parent_class " is not initialized.")) {                 \
			return nullptr;                                                    \
		}                                                                      \
		Ref<m_class> settings = parent->m_getter();                            \
		if (!ENSURE(is_valid(settings), #m_class " is not initialized.")) {    \
			return nullptr;                                                    \
		}                                                                      \
		return settings;                                                       \
	}                                                                          \
                                                                               \
	Ref<m_class> m_class::get_maybe() {                                        \
		Ref<m_parent_class> parent = m_parent_class::get();                    \
		if (is_valid(parent)) {                                                \
			Ref<m_class> settings = parent->m_getter();                        \
			if (is_valid(settings)) {                                          \
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
