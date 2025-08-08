#ifndef SNORE_CORE_MAIN_MODULE_H
#define SNORE_CORE_MAIN_MODULE_H

#include "snore_core/canvas_layer_config.h"
#include "snore_core/internal/ref_utils.h"
#include "snore_core/snore_core_main_settings.h"
#include "snore_core/snore_core_root_module.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/variant/typed_array.hpp>

namespace godot {

class SceneTree;
class Viewport;
class Node;

class SnoreCore : public SnoreCoreRootModule<SnoreCoreMainSettings> {
	GDCLASS(SnoreCore, SnoreCoreRootModule)
	SC_ROOT_MODULE_CLASS(SnoreCore, SnoreCoreMainSettings)

public:
	static void register_gdextension_types(ModuleInitializationLevel p_level);
	static void unregister_gdextension_types(ModuleInitializationLevel p_level);

	static bool are_tests_enabled();
	static bool get_are_tests_running() { return are_tests_running; }
	static bool run_tests();

	static void set_up_from_binding(
			const TypedArray<SnoreCoreSettings> &p_all_settings);

	static SnoreCoreRootModule *get_module(const StringName &p_name) {
		SnoreCore *main = SnoreCore::get();
		if (!ENSURE(main, "SnoreCore is not initialized.")) {
			return nullptr;
		}
		if (!ENSURE(main->modules.find(p_name) != main->modules.end(),
					"Module not found: " + p_name)) {
			return nullptr;
		}
		return main->modules[p_name];
	}

	static TypedArray<SnoreCoreRootModule> get_modules() {
		SnoreCore *main = SnoreCore::get();
		if (!ENSURE(main, "SnoreCore is not initialized.")) {
			return TypedArray<SnoreCoreRootModule>();
		}

		TypedArray<SnoreCoreRootModule> result;
		result.resize(main->modules.size());

		for (const std::pair<const StringName, SnoreCoreRootModule *> &pair :
			 main->modules) {
			result.push_back(pair.second);
		}

		return result;
	}

	SnoreCore() = default;
	~SnoreCore() {
		// Clear the modules map to ensure proper cleanup.
		modules.clear();
	}

	void set_up_main(const TypedArray<SnoreCoreSettings> &p_all_settings);

	void on_module_set_up_finished(const StringName &p_name);

	void register_module(Object *p_module);
	void unregister_module(Object *p_module);

	bool is_modules_empty() const { return modules.empty(); }

	uint64_t get_last_set_up_time_msec() const { return last_set_up_time_msec; }
	void set_last_set_up_time_msec(uint64_t p_value) {
		last_set_up_time_msec = p_value;
	}

	SceneTree *get_scene_tree() const;
	Viewport *get_viewport() const;
	void add_utility_node(Node *p_node, const StringName &p_name);

protected:
	static void _bind_methods();

private:
	static bool are_types_registered;
	static bool are_tests_running;

	std::unordered_map<StringName, SnoreCoreRootModule *> modules;

	uint64_t last_set_up_time_msec = 0;

	SnoreCoreRootModule *get_module_for_settings(
			const StringName &p_settings_name) const {
		for (const std::pair<const StringName, SnoreCoreRootModule *> &pair :
			 modules) {
			// FIXME: Test that get_class_name() works as expected. Else, record
			//        name on settings class with macro.
			if (pair.second->get_settings_class_name() == p_settings_name) {
				return pair.second;
			}
		}
		ENSURE(false, "Module not found for settings: " + p_settings_name);
		return nullptr;
	}
};

} //namespace godot

#endif // SNORE_CORE_MAIN_MODULE_H
