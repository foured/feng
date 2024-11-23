#include "transform.h"

#include <iostream>

namespace feng {

	transform::transform(glm::vec3 pos, glm::vec3 size, glm::vec3 rotation)
		: _position(pos), _size(size), _rotation(rotation) {}

	void transform::set_position(glm::vec3 val) {
		_position = val;
	}

	void transform::set_size(glm::vec3 val) {
		_size = val;
	}

	void transform::set_rotation(glm::vec3 val) {
		_rotation = val;
	}
	
	glm::vec3 transform::get_position() const {
		return _position;
	}

	glm::vec3 transform::get_size() const {
		return _size;
	}

	glm::vec3 transform::get_rotation() const {
		return _rotation;
	}

}