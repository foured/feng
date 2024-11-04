#include "transform.h"

namespace feng {

	transform::transform(glm::vec3 pos, glm::vec3 size, glm::vec3 rotation)
		: _position(pos), _size(size), _rotation(rotation) {}

}