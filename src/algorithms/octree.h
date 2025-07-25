#pragma once

#include <glm/glm.hpp>

#include <array>
#include <vector>
#include <memory>

#include "aabb.h"
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

	class object_type {
	public:
		object_type(std::shared_ptr<simple_collider> object);

		bool is_static() const;
		bool expired() const;
		bool is_inserted() const;
		void set_inserted();

		simple_collider* get() const;
		std::shared_ptr<simple_collider> get_sp() const;
		simple_collider* operator->();
		const simple_collider* operator->() const;

	private:
		std::weak_ptr<simple_collider> _object;
		bool _static = false;
		bool _inserted = false;

	};

	class node {
	public:
		node(glm::vec3 pos, float width);
		~node();
		// TODO: add constructor with bounds

		bool add_insance(std::shared_ptr<simple_collider> object);
		bool add_insance(object_type* object);

		void update();
		void process_left_objects();
		void check_collisions();

		uint32_t find_instance(std::shared_ptr<simple_collider> instance);

		void delete_unused_children();

	private:                                     //  s  a  (size, alignment)
		std::array<node*, NUM_OCTANTS> _octants; //  64 8
		std::vector<object_type*> _left_objects; //  32 8
		std::vector<object_type*> _objects;		 //  32 8
		node* _parent = nullptr;				 //  8  8
		aabb _bounds;							 //  24 4
		bool _last_node = false;				 //  1  1
		bool _root = false;                      //  1  1
		uint8_t _no_children = 0;                //  1  1
		int8_t _time_to_death = OCTREE_LIFESPAN; //  1  1

		node(glm::vec3 pos, float width, node* parent);
		
		void check_against(const std::vector<object_type*>& strangers);
		void check_against(object_type* stranger);
		void on_collision(object_type* object1, object_type* object2);
		void check_pair_collision(object_type* object1, object_type* object2);
		// dynamic and static
		void check_pair_collision_ds(object_type* d, object_type* s);

		bool add_instance_upwards(object_type* object);
		void add_contained_instance(object_type* object);

		void add_to_optimal_intersecting_node(object_type* object);
		void push_object(object_type* object);
		node* generate_octant(octants specification);
		node* generate_octant(uint8_t id);
		void revive_dead_octants();
		bool are_octants_empty();

		bool empty() const;
		bool contains(object_type* object) const;
		bool intersects(object_type* object) const;
		bool can_fit(object_type* object) const;
		aabb octant_size() const;

	};

}