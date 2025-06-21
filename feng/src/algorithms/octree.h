#pragma once

#include <glm/glm.hpp>

#include <array>
#include <vector>
#include <memory>

#include "../logic/aabb.h"
#include "../logic/world/components/simple_collider.h"

#define MIN_OCTREE_WIDTH 0.25
#define NUM_OCTANTS 8 // ALWAYS 8
#define OCTREE_POPULATION_LIMIT 2
#define OCTREE_LIFESPAN 10 // in frames

namespace feng::octree {
	using obj_type = std::shared_ptr<simple_collider>;

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

	class object_type {
	public:
		object_type(std::shared_ptr<simple_collider> object);

		bool is_static() const;

		simple_collider* get() const;
		simple_collider* operator->();
		const simple_collider* operator->() const;

	private:
		std::weak_ptr<simple_collider> _object;
		bool _is_static;

	};

	class node {
	public:
		node(glm::vec3 pos, float width);
		~node();
		// TODO: add constructor with bounds

		bool add_insance(obj_type object);
		void update();
			
		void delete_unused_children();

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
		
		void check_collisions();
		void check_against(const std::vector<obj_type>& strangers);
		void on_collision(obj_type object1, obj_type object2);

		void add_to_optimal_intersecting_node(obj_type object);
		void push_object(obj_type object);
		node* generate_octant(octants specification);
		node* generate_octant(uint8_t id);
		void revive_dead_octants();
		bool are_octants_empty();

		bool empty() const;
		bool contains(const obj_type& object) const;
		bool intersects(const obj_type& object) const;
		bool can_fit(const obj_type& object) const;
		aabb octant_size() const;

	};

}