#pragma once

#include <glm/glm.hpp>

#include <array>
#include <vector>

#include "../logic/aabb.h"
#include "../logic/world/components/simple_collider.h"

#define MIN_OCTREE_WIDTH 0.25
#define NUM_OCTANTS 8 // ALWAYS 8
#define OCTREE_POPULATION_LIMIT 2
#define OCTREE_LIFESPAN 10 // in frames

namespace feng::octree {

	enum class octants : uint8_t {
		LBB = 0, // Left  Bottom Back
		LBF = 1, // Left  Bottom Front
		LTB = 2, // Left  Top    Back
		LTF = 3, // Left  Top    Front
		RBB = 4, // Right Bottom Back
		RBF = 5, // Right Bottom Front
		RTB = 6, // Right Top    Back
		RTF = 7  // Right Top    Front
	};


	class node {
	public:
		using obj_type = std::shared_ptr<simple_collider>;

		node(glm::vec3 pos, float width);
		~node();
		// TODO: add constructor with bounds

		bool add_insance(obj_type object);
		void update();

	private:
		aabb _bounds;
		node* _parent = nullptr;
		std::array<node*, NUM_OCTANTS> _octants;
		std::vector<obj_type> _objects;
		bool _last_node = false;
		bool _root = false;
		uint8_t _no_children = 0;
		int8_t _time_to_death = OCTREE_LIFESPAN;

		node(glm::vec3 pos, float width, node* parent);

		bool try_add_instance_upward(obj_type object);
		void add_instance_good_conditions(obj_type object);
		node* generate_octant(octants specification);
		bool contains(obj_type object) const;

	};

}