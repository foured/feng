#pragma once

#include <glm/glm.hpp>

#include <array>

#include "../logic/aabb.h"

#define MIN_OCTREE_WIDTH 0.25

namespace feng::octree {

	class node {
	public:
		node(glm::vec3 pos, float width);

		void add_insance();

	private:
		aabb _bounds;
		node* _parent = nullptr;
		std::array<node*, 8> _children;

	};

}