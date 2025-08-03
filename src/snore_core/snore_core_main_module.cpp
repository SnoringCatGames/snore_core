#include "snore_core/snore_core_main_module.h"

#include "snore_core/annotation.h"
#include "snore_core/annotations_service.h"
#include "snore_core/canvas_layer_config.h"
#include "snore_core/canvas_layer_service.h"
#include "snore_core/circular_buffer.h"
#include "snore_core/geometry.h"
#include "snore_core/internal/registration_utils.h"
#include "snore_core/internal/snore_core_module_utils.h"
#include "snore_core/log_service.h"
#include "snore_core/rotated_shape.h"
#include "snore_core/snore_core_main_settings.h"
#include "snore_core/snore_core_root_module.h"
#include "snore_core/snore_core_settings.h"
#include "snore_core/snore_core_utils.h"
#include "snore_core/time/stopwatch.h"
#include "snore_core/time/time_debouncer.h"
#include "snore_core/time/time_interval.h"
#include "snore_core/time/time_service.h"
#include "snore_core/time/time_throttler.h"
#include "snore_core/time/time_timeout.h"
#include "snore_core/time/time_tracker.h"
#include "snore_core/time/time_type.h"

#include <godot_cpp/classes/Node.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>

// Only include test files in debug builds.
#ifdef SC_TESTS_ENABLED
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "snore_core/internal/test_internal_string_utils.h"
#include "snore_core/test_annotation.h"
#include "snore_core/test_annotations_service.h"
#include "snore_core/test_canvas_layer_config.h"
#include "snore_core/test_canvas_layer_service.h"
#include "snore_core/test_circular_buffer.h"
#include "snore_core/test_geometry.h"
#include "snore_core/test_log_service.h"
#include "snore_core/test_rotated_shape.h"
#include "snore_core/test_snore_core_main_module.h"
#include "snore_core/test_snore_core_main_settings.h"
#include "snore_core/test_snore_core_root_module.h"
#include "snore_core/test_snore_core_settings.h"
#include "snore_core/test_snore_core_utils.h"
#include "snore_core/time/test_stopwatch.h"
#include "snore_core/time/test_time_debouncer.h"
#include "snore_core/time/test_time_interval.h"
#include "snore_core/time/test_time_service.h"
#include "snore_core/time/test_time_throttler.h"
#include "snore_core/time/test_time_timeout.h"
#include "snore_core/time/test_time_tracker.h"
#endif // SC_TESTS_ENABLED

using namespace godot;

bool SnoreCore::are_types_registered = false;
bool SnoreCore::are_tests_running = false;

void SnoreCore::register_gdextension_types(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	// This method is idempotent, so we check here whether it has been called
	// already.
	if (are_types_registered) {
		return;
	}
	are_types_registered = true;

	GDREGISTER_ABSTRACT_CLASS(SnoreCoreSettings);
	GDREGISTER_ABSTRACT_CLASS(SnoreCoreRootModule);
	GDREGISTER_VIRTUAL_CLASS(Annotation);

	GDREGISTER_CLASS(AnnotationsService);
	GDREGISTER_CLASS(CanvasLayerConfig);
	GDREGISTER_CLASS(CanvasLayerService);
	GDREGISTER_CLASS(CircularBuffer);
	GDREGISTER_CLASS(Geometry);
	GDREGISTER_CLASS(LogService);
	GDREGISTER_CLASS(RotatedShape);
	GDREGISTER_CLASS(TimeService);
	GDREGISTER_CLASS(SnoreCoreUtils);
	GDREGISTER_CLASS(SnoreCore);
	GDREGISTER_CLASS(SnoreCoreMainSettings);
	GDREGISTER_CLASS(Stopwatch);
	GDREGISTER_CLASS(TimeDebouncer);
	GDREGISTER_CLASS(TimeInterval);
	GDREGISTER_CLASS(TimeThrottler);
	GDREGISTER_CLASS(TimeTimeout);
	GDREGISTER_CLASS(TimeTracker);
	GDREGISTER_CLASS(TimeType);

	// Only include test classes in debug builds.
#ifdef SC_TESTS_ENABLED
	GDREGISTER_CLASS(FakeSnoreCoreSettings);
	GDREGISTER_CLASS(FakeSnoreCoreModule);
#endif // SC_TESTS_ENABLED

	snore_core_module_utils_internal::
			register_snore_core_main_module_if_not_present();
}

void SnoreCore::unregister_gdextension_types(
		ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	// Unregister all modules.
	SnoreCore *main = SnoreCore::get();
	if (main) {
		// Unregister all other modules before SnoreCore.
		for (const std::pair<const StringName, SnoreCoreRootModule *> &pair :
			 main->modules) {
			if (pair.first != main->get_name()) {
				pair.second->reset();
				unregister_engine_singleton(pair.first);
			}
		}
		unregister_engine_singleton(SnoreCore::name);
	}
}

void SnoreCore::set_up_from_binding(
		const TypedArray<SnoreCoreSettings> &p_all_settings) {
	SnoreCore *main = SnoreCore::get();
	if (!ENSURE(main, "SnoreCore is not initialized.")) {
		return;
	}
	main->set_up_main(p_all_settings);
}

void SnoreCore::set_up_main(
		const TypedArray<SnoreCoreSettings> &p_all_settings) {
	// Check that we're only setting up once at the start of the app.
	const Time *time = Time::get_singleton();
	const uint64_t current_time_msec = time->get_ticks_msec();
	// In tests, we may call set_up multiple times in quick succession.
#ifndef SC_TESTS_ENABLED
	ENSURE(current_time_msec > last_set_up_time_msec + 500,
		   "set_up should only be called once at the start of the app.");
#endif // SC_TESTS_ENABLED
	last_set_up_time_msec = current_time_msec;

	for (int i = 0; i < p_all_settings.size(); ++i) {
		SnoreCoreSettings *settings =
				Object::cast_to<SnoreCoreSettings>(p_all_settings[i]);
		if (!ENSURE(settings,
					"Element in settings array is not a SnoreCoreSettings.")) {
			continue;
		}
		// FIXME: Test that get_class() works as expected. Else, record
		//        name on settings class with macro.
		SnoreCoreRootModule *module =
				get_module_for_settings(settings->get_class());
		module->set_up_base(module->cast_to_settings(settings));
	}
}

std::vector<SnoreCoreSubmodule *> SnoreCore::instantiate_submodules() {
	return {
		memnew(LogService),
		memnew(SnoreCoreUtils),
		memnew(TimeService),
		memnew(AnnotationsService),
	};
}

void SnoreCore::set_up() {}

void SnoreCore::reset() {
	// Clear all modules to ensure proper cleanup.
	static const StringName snore_core_name = StringName(SnoreCore::name);
	for (auto &pair : modules) {
		if (pair.second && pair.first != snore_core_name) {
			pair.second->reset_base();
		}
	}
}

void SnoreCore::on_module_set_up_finished(const StringName &p_name) {
	SnoreCoreRootModule *module = get_module(p_name);
	if (!ENSURE_SIMPLE(module)) {
		return;
	}

	if (!ENSURE_SIMPLE(module->get_set_up_phase() == SET_UP_PHASE::FINISHED)) {
		return;
	}

	if (p_name != StringName(SnoreCore::name)) {
		// For non-SnoreCore modules, emit the signal now, before a possible
		// early-out.
		emit_signal("module_set_up_finished", p_name);
	}

	for (const std::pair<const StringName, SnoreCoreRootModule *> &pair :
		 modules) {
		if (!pair.second->get_is_set_up_finished() &&
			pair.first != StringName(SnoreCore::name)) {
			return;
		}
	}

	on_set_up_finished();

	// TODO: Log that the frameworks are set up.

	// For the SnoreCore module, emit the signal now, after confirming that all
	// other modules are finished.
	emit_signal("module_set_up_finished", SnoreCore::name);

	emit_signal("all_modules_set_up_finished");
}

void SnoreCore::register_module(Object *p_module) {
	SnoreCoreRootModule *module = static_cast<SnoreCoreRootModule *>(p_module);
	if (!ENSURE(module, "Cannot register a null module.")) {
		return;
	}

	const StringName module_name = module->get_name();
	modules[module_name] = module;
}

void SnoreCore::unregister_module(Object *p_module) {
	SnoreCoreRootModule *module = static_cast<SnoreCoreRootModule *>(p_module);
	if (!ENSURE(module, "Cannot unregister a null module.")) {
		return;
	}

	const StringName module_name = module->get_name();

	// Check if the module exists in our map before proceeding.
	if (!ENSURE(modules.find(module_name) != modules.end(),
				"Module not found in registry: " + module_name)) {
		return;
	}

	// Reset the module before removing it to ensure proper cleanup.
	module->reset_base();

	modules.erase(module_name);
}

SceneTree *SnoreCore::get_scene_tree() const {
	SceneTree *tree = Object::cast_to<SceneTree>(
			Engine::get_singleton()->get_main_loop());
	if (!ENSURE(tree, "SceneTree is not available.")) {
		return nullptr;
	}
	return tree;
}

Viewport *SnoreCore::get_viewport() const {
	SceneTree *tree = Object::cast_to<SceneTree>(
			Engine::get_singleton()->get_main_loop());
	if (!ENSURE(tree, "SceneTree is not available.")) {
		return nullptr;
	}
	Window *root = tree->get_root();
	if (!ENSURE(root, "Root window is not available.")) {
		return nullptr;
	}
	return root->get_viewport();
}

void SnoreCore::add_utility_node(Node *p_node, const StringName &p_name) {
	p_node->set_name(p_name);
	// FIXME: LEFT OFF HERE
	// SceneTree *tree = get_scene_tree();
	// if (tree) {
	// 	Window *root = tree->get_root();
	// 	if (root) {
	// 		root->add_child(p_node);
	// 	}
	// }
}

bool SnoreCore::are_tests_enabled() {
#ifdef SC_TESTS_ENABLED
	return true;
#else
	return false;
#endif
}

bool SnoreCore::run_tests() {
#ifdef SC_TESTS_ENABLED
	// Only list passing tests when running in CI.
#ifdef SC_CI_ENABLED
	char *brief_flag = "--gtest_brief=0";
#else
	char *brief_flag = "--gtest_brief=1";
#endif // SC_CI_ENABLED

	are_tests_running = true;

	int argc = 2;
	char *argv[] = { "dummy", brief_flag };

	testing::InitGoogleMock(&argc, argv);

	const bool did_all_tests_pass = RUN_ALL_TESTS() == 0;

	are_tests_running = false;

	// NOTE: The GitHub Actions CI checks for the text "SnoreCore test result"
	//       in order to determine whether the tests passed or failed.
	Log::empty_line();
	if (did_all_tests_pass) {
		Log::print("SnoreCore test result: ALL TESTS PASSED!");
	} else {
		Log::print("SnoreCore test result: SOME TESTS FAILED!");
		ENSURE_SIMPLE(false);
	}
	Log::empty_line();

	return did_all_tests_pass;
#else
	Log::empty_line();
	Log::print("SnoreCore test result: TESTS NOT INCLUDED IN RELEASE BUILDS!");
	Log::empty_line();
	return false;
#endif // SC_TESTS_ENABLED
}

void SnoreCore::_bind_methods() {
	ClassDB::bind_static_method(
			name, D_METHOD("set_up", "p_settings"),
			&SnoreCore::set_up_from_binding);
	ClassDB::bind_static_method(
			name, D_METHOD("get_module", "p_name"), &SnoreCore::get_module);
	ClassDB::bind_static_method(
			name, D_METHOD("get_modules"), &SnoreCore::get_modules);
	ClassDB::bind_static_method(
			name, D_METHOD("are_tests_enabled"), &SnoreCore::are_tests_enabled);
	ClassDB::bind_static_method(
			name, D_METHOD("run_tests"), &SnoreCore::run_tests);

	ADD_SIGNAL(MethodInfo(
			"module_set_up_finished",
			PropertyInfo(Variant::STRING_NAME, "name")));
	ADD_SIGNAL(MethodInfo("all_modules_set_up_finished"));

	ClassDB::bind_method(D_METHOD("get_settings"), &SnoreCore::get_settings);
}
