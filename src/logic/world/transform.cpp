#include "transform.h"

#include <iostream>

#include "../../utilities/utilities.h"

namespace feng {

	transform::transform(const glm::vec3& pos, const glm::vec3& size, const glm::vec3& rotation)
		: _position(pos), _size(size), _rotation(rotation) { }

	void transform::set_position(const glm::vec3& val) {
		_position = val;
		changed_this_frame = true;
	}

	void transform::set_size(const glm::vec3& val) {
		_size = val;
		changed_this_frame = true;
	}

	void transform::set_rotation(const glm::vec3& val) {
		_rotation = val;
		_rotation_matrix = utilities::euler2mat3x3(val);
		changed_this_frame = true;
	}
	
	void transform::add_position(const glm::vec3& offset) {
		_position += offset;
		changed_this_frame = true;
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

	glm::mat3 transform::get_rotation_matrix3x3() const {
		return _rotation_matrix;
	}

}