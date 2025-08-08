#ifndef SNORE_CORE_ROOT_MODULE_H
#define SNORE_CORE_ROOT_MODULE_H

#include "snore_core/internal/debug_utils.h"
#include "snore_core/internal/ref_utils.h"
#include "snore_core/snore_core_settings.h"
#include "snore_core/snore_core_submodule.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/typed_array.hpp>

#define SC_ROOT_MODULE_CLASS(m_class, m_settings_class)                        \
public:                                                                        \
	static const constexpr char *name = #m_class;                              \
                                                                               \
	static m_class *get() {                                                    \
		m_class *instance = get_maybe();                                       \
		if (!ENSURE(instance, #m_class " is not initialized.")) {              \
			return nullptr;                                                    \
		}                                                                      \
		return instance;                                                       \
	}                                                                          \
                                                                               \
	static m_class *get_maybe() {                                              \
		Engine *engine = Engine::get_singleton();                              \
		return engine->has_singleton(m_class::name)                            \
				? static_cast<m_class *>(engine->get_singleton(m_class::name)) \
				: nullptr;                                                     \
	}                                                                          \
                                                                               \
	virtual const StringName &get_name() const override {                      \
		static const StringName string_name = StringName(name);                \
		return string_name;                                                    \
	}                                                                          \
                                                                               \
	virtual const StringName &get_settings_class_name() const override {       \
		return m_settings_class::get_class_static();                           \
	}                                                                          \
                                                                               \
	virtual Ref<m_settings_class> cast_to_settings(                            \
			const Ref<RefCounted> &p_object) const override {                  \
		return Object::cast_to<m_settings_class>(p_object.ptr());              \
	}                                                                          \
                                                                               \
	virtual void set_settings(const Ref<m_settings_class> &p_settings) {       \
		settings = p_settings;                                                 \
	}                                                                          \
                                                                               \
	/* TODO: This probably shouldn't be needed, but the Binding logic          \
			 complains about duplicates when binding to the generic parent     \
			 version. */                                                       \
	Ref<m_settings_class> get_settings() const { return settings; }            \
                                                                               \
	virtual void set_up() override;                                            \
                                                                               \
	virtual void reset() override;                                             \
                                                                               \
protected:                                                                     \
	virtual std::vector<SnoreCoreSubmodule *> instantiate_submodules() override;

namespace godot {

template <typename SettingsType> class SnoreCoreRootModule;

namespace SnoreCoreModuleInternal {
void GDE_EXPORT
notify_main_module_of_module_set_up_finished(const StringName &p_name);
} // namespace SnoreCoreModuleInternal

template <typename SettingsType> class SnoreCoreRootModule : public Object {
	GDCLASS(SnoreCoreRootModule, Object)

	static_assert(
			std::is_base_of<SnoreCoreSettings, SettingsType>::value,
			"SettingsType must be derived from SnoreCoreSettings");

public:
	enum SET_UP_PHASE {
		NOT_STARTED,
		IN_PROGRESS,
		FINISHED,
	};

	SnoreCoreRootModule() { settings = instantiate_ref<SettingsType>(); }
	virtual ~SnoreCoreRootModule() = default;

	virtual const StringName &get_name() const = 0;
	virtual const StringName &get_settings_class_name() const = 0;
	virtual Ref<SettingsType> cast_to_settings(
			const Ref<RefCounted> &p_object) const = 0;
	virtual void set_settings(const Ref<SettingsType> &p_settings) = 0;

	// This is called during game runtime, after settings are loaded.
	virtual void set_up() = 0;

	// This is called during game runtime, before calling set_up.
	virtual void reset() = 0;

	// This resets some base state before calling reset().
	void reset_base() {
		settings = Ref<SettingsType>();
		set_up_phase = SET_UP_PHASE::NOT_STARTED;
		reset_submodules();
		reset();
	}

	// This sets some tracking state before calling set_up().
	void set_up_base(Ref<SettingsType> &p_settings) {
		reset_base();
		set_up_phase = SET_UP_PHASE::IN_PROGRESS;
		set_settings(p_settings);
		set_up_submodules();
		set_up();
	}

	SET_UP_PHASE get_set_up_phase() const { return set_up_phase; }

	bool get_is_set_up_started() const {
		return set_up_phase == SET_UP_PHASE::IN_PROGRESS ||
				set_up_phase == SET_UP_PHASE::FINISHED;
	}

	bool get_is_set_up_finished() const {
		return set_up_phase == SET_UP_PHASE::FINISHED;
	}

	// FIXME: Remove if not used.
	Ref<SettingsType> get_generic_settings() const { return settings; }

	SnoreCoreSubmodule *get_submodule(const StringName &p_name) const {
		auto it = submodules.find(p_name);
		if (it != submodules.end()) {
			return it->second;
		}
		return nullptr;
	}

protected:
	SET_UP_PHASE set_up_phase = SET_UP_PHASE::NOT_STARTED;

	Ref<SettingsType> settings;

	std::unordered_map<StringName, SnoreCoreSubmodule *> submodules;

	static void _bind_methods() {}

	// This is called during game runtime, after settings are loaded.
	virtual std::vector<SnoreCoreSubmodule *> instantiate_submodules() = 0;

	void on_set_up_finished() {
		if (!ENSURE(set_up_phase == SET_UP_PHASE::IN_PROGRESS,
					"Cannot finish set_up when it is not in progress.")) {
			return;
		}

		set_up_phase = SET_UP_PHASE::FINISHED;

		SnoreCoreModuleInternal::notify_main_module_of_module_set_up_finished(
				get_name());
	}

	void register_submodule(SnoreCoreSubmodule *p_submodule) {
		if (!ENSURE(!get_is_set_up_finished(),
					"Cannot add submodule after setup is finished.")) {
			return;
		}

		if (!ENSURE(p_submodule, "Cannot add a null submodule.")) {
			return;
		}

		const StringName name = p_submodule->get_name();
		if (!ENSURE(submodules.find(name) == submodules.end(),
					"Submodule with name '" + name +
							"' already exists in this module.")) {
			return;
		}

		submodules[name] = p_submodule;
	}

	void set_up_submodules() {
		// We initialize each submodule after registering each submodule to
		// support cross-submodule dependencies.
		const std::vector<SnoreCoreSubmodule *> submodules =
				instantiate_submodules();
		for (SnoreCoreSubmodule *submodule : submodules) {
			register_submodule(submodule);
		}
		for (SnoreCoreSubmodule *submodule : submodules) {
			submodule->reset_base();
			submodule->set_up_base();
		}
		on_set_up_finished();
	}

	void reset_submodules() {
		for (std::pair<const StringName, SnoreCoreSubmodule *> submodule :
			 submodules) {
			submodule.second->reset_base();
			memdelete(submodule.second);
		}
		submodules.clear();
	}
};

} //namespace godot

#endif // SNORE_CORE_ROOT_MODULE_H
