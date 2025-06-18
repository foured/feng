#include "octree.h"

namespace feng::octree {

	node::node(glm::vec3 pos, float width) {
		glm::vec3 offset(width / 2.0f);
		_bounds = aabb(pos - offset, pos + offset);
		_children.fill(nullptr);
	}

}