#ifndef SNORE_CORE_INTERNAL_STD_HASH_H
#define SNORE_CORE_INTERNAL_STD_HASH_H

// std::hash<godot::StringName> specialization so that StringName can
// be used as a key in std::unordered_map / std::unordered_set.
//
// godot-cpp 4.4 switched its internal containers from std::unordered_map
// to its own HashMap / AHashMap (see godot_cpp/core/class_db.hpp), so it
// no longer transitively provides a std::hash specialization for
// StringName. Code that uses STL hashed containers with StringName keys
// must include this header.
//
// The idiomatic long-term fix is to switch affected sites to
// godot::HashMap<StringName, T>. Tracked in bootstrapper's ROADMAP under
// "Switch std::unordered_map<StringName, ...> to godot::HashMap".

#include <cstddef>
#include <cstdint>
#include <functional>

#include <godot_cpp/variant/string_name.hpp>

namespace std {

template <>
struct hash<godot::StringName> {
	std::size_t operator()(const godot::StringName &p_name) const noexcept {
		return static_cast<std::size_t>(
				static_cast<std::uint64_t>(p_name.hash()));
	}
};

} // namespace std

#endif // SNORE_CORE_INTERNAL_STD_HASH_H
