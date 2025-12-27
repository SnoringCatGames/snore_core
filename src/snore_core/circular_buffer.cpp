#include "snore_core/circular_buffer.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void CircularBuffer::initialize(int32_t p_max_size) {
	set_max_size(p_max_size);
}

void CircularBuffer::push(const Variant &p_value) {
	if (max_size <= 0) {
		return;
	}

	buffer.write[head] = p_value;
	head = (head + 1) % max_size;
	current_size = MIN(current_size + 1, max_size);
}

Variant CircularBuffer::get(int32_t p_index) const {
	if (p_index < 0 || p_index >= current_size) {
		return Variant();
	}

	if (current_size < max_size) {
		// Buffer is not full yet, use direct indexing.
		return buffer[p_index];
	} else {
		// Buffer is full, calculate the actual index.
		int32_t actual_index = (head + p_index) % max_size;
		return buffer[actual_index];
	}
}

void CircularBuffer::clear() {
	buffer.clear();
	head = 0;
	current_size = 0;
}

void CircularBuffer::set_max_size(int32_t p_max_size) {
	if (p_max_size < 0) {
		p_max_size = 0;
	}

	clear();

	max_size = p_max_size;
	if (max_size > 0) {
		buffer.resize(max_size);
	}
}

void CircularBuffer::_bind_methods() {
	ClassDB::bind_method(
			D_METHOD("initialize", "p_max_size"), &CircularBuffer::initialize);

	ClassDB::bind_method(D_METHOD("push", "p_value"), &CircularBuffer::push);

	ClassDB::bind_method(D_METHOD("get", "p_index"), &CircularBuffer::get);

	ClassDB::bind_method(D_METHOD("size"), &CircularBuffer::size);

	ClassDB::bind_method(D_METHOD("capacity"), &CircularBuffer::capacity);

	ClassDB::bind_method(D_METHOD("clear"), &CircularBuffer::clear);

	ClassDB::bind_method(
			D_METHOD("get_max_size"), &CircularBuffer::get_max_size);
	ClassDB::bind_method(
			D_METHOD("set_max_size", "p_max_size"),
			&CircularBuffer::set_max_size);
	ADD_PROPERTY(
			PropertyInfo(Variant::INT, "max_size"), "set_max_size",
			"get_max_size");
}
