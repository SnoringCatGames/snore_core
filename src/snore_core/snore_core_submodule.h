#ifndef SNORE_CORE_SUBMODULE_H
#define SNORE_CORE_SUBMODULE_H

#include "snore_core/internal/ref_utils.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>

#define SC_SUBMODULE_CLASS(m_class, m_root_module_class)                       \
public:                                                                        \
	static const constexpr char *name = #m_class;                              \
                                                                               \
	static m_class *get() {                                                    \
		m_root_module_class *root = m_root_module_class::get();                \
		if (!ENSURE(root, #m_root_module_class " is not initialized.")) {      \
			return nullptr;                                                    \
		}                                                                      \
		m_class *instance =                                                    \
				Object::cast_to<m_class>(root->get_submodule(name));           \
		if (!ENSURE(instance, #m_class " is not initialized.")) {              \
			return nullptr;                                                    \
		}                                                                      \
		return instance;                                                       \
	}                                                                          \
                                                                               \
	static m_class *get_maybe() {                                              \
		m_root_module_class *root = m_root_module_class::get_maybe();          \
		if (!root) {                                                           \
			return nullptr;                                                    \
		}                                                                      \
		m_class *instance =                                                    \
				Object::cast_to<m_class>(root->get_submodule(name));           \
		if (!instance) {                                                       \
			return nullptr;                                                    \
		}                                                                      \
		return instance;                                                       \
	}                                                                          \
                                                                               \
	virtual const StringName &get_name() const override {                      \
		static const StringName string_name = StringName(name);                \
		return string_name;                                                    \
	}                                                                          \
                                                                               \
	virtual void set_up() override;                                            \
                                                                               \
	virtual void reset() override;

#define SC_SUBMODULE_WITH_NODE_CLASS(                                          \
		m_class, m_root_module_class, m_node_name, m_node_type)                \
	SC_SUBMODULE_CLASS(m_class, m_root_module_class)                           \
                                                                               \
public:                                                                        \
	static const constexpr char *node_name = m_node_name;                      \
                                                                               \
	virtual const StringName &get_node_name() const override {                 \
		static const StringName string_name = StringName(node_name);           \
		return string_name;                                                    \
	}                                                                          \
                                                                               \
	virtual void instantiate_node() override {                                 \
		node = memnew(m_node_type);                                            \
		SnoreCore::get()->add_utility_node(node, m_node_name);                 \
	}                                                                          \
                                                                               \
	virtual void reset_node() override {                                       \
		if (is_instance_valid(node)) {                                         \
			node->queue_free();                                                \
			node = nullptr;                                                    \
		}                                                                      \
	}                                                                          \
                                                                               \
private:                                                                       \
	m_node_type *node = nullptr;

namespace godot {

class SnoreCoreSubmodule : public Object {
	GDCLASS(SnoreCoreSubmodule, Object)

public:
	SnoreCoreSubmodule() = default;
	virtual ~SnoreCoreSubmodule() = default;

	virtual const StringName &get_name() const = 0;

	void set_up_base();

	void reset_base();

protected:
	static void _bind_methods() {}

	// This is called during game runtime, after settings are loaded.
	virtual void set_up() = 0;

	// This is called during game runtime, before calling set_up.
	virtual void reset() = 0;

	// - This is called immediately before setup.
	// - Only override this if you need to set some custom node state.
	//   Otherwise, the SC_SUBMODULE_WITH_NODE_CLASS macro will automatically
	//   handle creating the node.
	virtual void instantiate_node() {}

	// - This is called immediately before reset.
	// - Only override this if you need to set some custom node state.
	//   Otherwise, the SC_SUBMODULE_WITH_NODE_CLASS macro will automatically
	//   handle creating the node.
	virtual void reset_node() {}
};

class SnoreCoreSubmoduleWithNode : public SnoreCoreSubmodule {
	GDCLASS(SnoreCoreSubmoduleWithNode, SnoreCoreSubmodule)

public:
	SnoreCoreSubmoduleWithNode() = default;
	virtual ~SnoreCoreSubmoduleWithNode() = default;

	virtual const StringName &get_node_name() const = 0;

protected:
	static void _bind_methods() {}
};

} //namespace godot

#endif // SNORE_CORE_SUBMODULE_H
