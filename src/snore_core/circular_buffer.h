#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/variant.hpp>

namespace godot {

class CircularBuffer : public RefCounted {
	GDCLASS(CircularBuffer, RefCounted)

public:
	CircularBuffer() = default;
	~CircularBuffer() = default;

	void initialize(int32_t p_max_size);
	void push(const Variant &p_value);
	Variant get(int32_t p_index) const;
	int32_t size() const { return current_size; }
	int32_t capacity() const { return max_size; }
	void clear();

	// Getters and setters.
	int32_t get_max_size() const { return max_size; }
	void set_max_size(int32_t p_max_size);

protected:
	static void _bind_methods();

private:
	Vector<Variant> buffer;
	int32_t max_size = 0;
	int32_t head = 0;
	int32_t current_size = 0;
};

} // namespace godot

#endif // CIRCULAR_BUFFER_H
