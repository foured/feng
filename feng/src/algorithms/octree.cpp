#include "octree.h"

#include "../logic/world/instance.h"
#include "../utilities/utilities.h"

namespace feng::octree {
	
	// OBJECT TYPE -----------------------------------------------------------------------------------------------------
	
	// PUBLIC-----------------------------------------------------------------------------------------------------------
	object_type::object_type(std::shared_ptr<simple_collider> object)
		: _object(object) {
		_is_static = object->get_instance()->flags.get(INST_FLAG_STATIC);
	}

	bool object_type::is_static() const {
		return _is_static;
	}

	simple_collider* object_type::get() const {
		auto ptr = _object.lock();
		FENG_ASSERT(ptr, "The octree object was destroyed, but someone is trying to work with it.");
		return ptr.get();
	}

	simple_collider* object_type::operator->() {
		return get();
	}

	const simple_collider* object_type::operator->() const {
		return get();
	}

	// NODE ------------------------------------------------------------------------------------------------------------

	// PUBLIC-----------------------------------------------------------------------------------------------------------
	node::node(glm::vec3 pos, float width) 
		: _root(true) {
		float hw = width / 2.0f;
		glm::vec3 offset(hw);
		_bounds = aabb(pos - offset, pos + offset);
		_octants.fill(nullptr);
		_last_node = hw < MIN_OCTREE_WIDTH;
	}

	node::~node() {
		for (auto& child : _octants) {
			if (child != nullptr) {
				delete child;
				child = nullptr;
			}
		}
	}

	bool node::add_insance(obj_type object) {
		if (!contains(object)) {
			return false;
		}
		
		push_object(object);

		if (_last_node || (_objects.size() <= OCTREE_POPULATION_LIMIT && are_octants_empty()))
			return true;

		revive_dead_octants();

		std::vector<obj_type> remaining;
		for (auto& obj : _objects) {
			bool inserted = false;
			for (auto& child : _octants) {
				if (child->add_insance(obj)) {
					inserted = true;
					break;
				}
			}
			if (!inserted)
				remaining.push_back(obj);
		}

		_objects.clear();

		if (remaining.size() > 0) {
			LOG_INFO("Separating objects: ", remaining.size());
			// check if can be stored in children
			std::vector<node*> intersections;
			for (auto& object : remaining) {
				for (auto& octant : _octants) {
					if (octant->intersects(object)) {
						intersections.push_back(octant);
					}
				}

				if (intersections.empty())
					THROW_ERROR("Object stands in octant but doesnt intersect with its children");

				if (intersections.size() < NUM_OCTANTS) {
					for (auto& octant : intersections) {
						//octant->push_object(object);
						octant->add_to_optimal_intersecting_node(object);
					}
				}
				else {
					_objects.push_back(object);
				}
				intersections.clear();
			}
		}

		return true;
	}

	void node::update() {
		utilities::test_octree_visualiser->add_instance(_bounds);

		// processing death
		if (!_root && _objects.size() == 0 && _no_children == 0) {
			_time_to_death -= 1;
		}

		for (uint8_t i = 0; i < NUM_OCTANTS; i++) {
			if (_octants[i] != nullptr) {
				if (_octants[i]->_time_to_death == 0) {
					if (_octants[i]->_no_children == 0 && _octants[i]->_objects.size() == 0) {
						_no_children--;
						delete _octants[i];
						_octants[i] = nullptr;
					}
					else {
						LOG_WARNING("Node with children or with objects has _time_to_death = 0");
					}
				}
				else {
					_octants[i]->update();
				}
			}
		}

		if (_objects.size() > 0) {
			// processing moving objects
			 
			//std::vector<obj_type> remaining;
			//for (auto& object : _objects) {
			//	if (object->update_bounds() && !contains(object)) {
			//		if (!_parent || !_parent->add_insance(object)) {
			//			LOG_WARNING("Object ", object->get_instance_uuid_string(), " moved out of octree bounds");
			//		}
			//	}
			//	else {
			//		remaining.push_back(object);
			//	}
			//}
			//if(remaining.size() != _objects.size())
			//	_objects = std::move(remaining);

			// processing collisions
			check_collisions();
		}
	}

	void node::delete_unused_children() {
		for (auto& octant : _octants) {
			if (octant != nullptr && octant->empty()) {
				delete octant;
			}
		}
	}

	// PRIVATE-------------------------------------------------------------------------------------------------------------

	node::node(glm::vec3 pos, float width, node* parent)
		: node(pos, width) {
		_parent = parent;
		_root = _parent == nullptr;
	}

	void node::on_collision(obj_type object1, obj_type object2) {

	}

	void node::check_collisions() {
		for (size_t i = 0; i < _objects.size(); ++i) {
			for (size_t j = i + 1; j < _objects.size(); ++j) {
				if (_objects[i]->intersects(_objects[j])) {
					on_collision(_objects[i], _objects[j]);
				}
			}
		}

		for (auto& octant : _octants) {
			if (octant != nullptr) {
				octant->check_against(_objects);
				octant->check_collisions();
			}
		}
	}

	void node::check_against(const std::vector<obj_type>& strangers) {
		for (const auto& stranger : strangers) {
			for (const auto& local : _objects) {
				if (stranger->intersects(local)) {
					on_collision(stranger, local);
				}
			}
		}

		for (auto& octant : _octants) {
			if (octant != nullptr) {
				octant->check_against(strangers);
			}
		}
	}

	void node::add_to_optimal_intersecting_node(obj_type object) {
		// _no_children == 0 || _last_node || 
		if (!can_fit(object) || _last_node || _no_children == 0) {
			push_object(object);
			return;
		}

		revive_dead_octants();

		for (const auto& octant : _octants)
			if (octant->intersects(object))
				octant->add_to_optimal_intersecting_node(object);
	}

	node* node::generate_octant(octants specification) {
		float half_size = _bounds.size().x / 2.0f;
		float quarter = half_size / 2.0f;
		glm::vec3 center = _bounds.center();

		glm::vec3 offset;

		switch (specification) {
		case octants::LBB: offset = { -quarter, -quarter, -quarter }; break; // Left  Bottom Back
		case octants::LBF: offset = { -quarter, -quarter,  quarter }; break; // Left  Bottom Front
		case octants::LTB: offset = { -quarter,  quarter, -quarter }; break; // Left  Top    Back
		case octants::LTF: offset = { -quarter,  quarter,  quarter }; break; // Left  Top    Front
		case octants::RBB: offset = { quarter,  -quarter, -quarter }; break; // Right Bottom Back
		case octants::RBF: offset = { quarter,  -quarter,  quarter }; break; // Right Bottom Front
		case octants::RTB: offset = { quarter,   quarter, -quarter }; break; // Right Top    Back
		case octants::RTF: offset = { quarter,   quarter,  quarter }; break; // Right Top    Front
		}

		glm::vec3 new_center = center + offset;
		_no_children++;
		return new node(new_center, half_size, this);
	}

	node* node::generate_octant(uint8_t id) {
		if (id < 0 || id >= NUM_OCTANTS)
			THROW_ERROR("Octant id is out of bounds: ", id);
		return generate_octant(static_cast<octants>(id));
	}

	void node::revive_dead_octants() {
		for (uint8_t i = 0; i < NUM_OCTANTS; i++) {
			if (_octants[i] == nullptr) {
				_octants[i] = generate_octant(i);
			}
		}
	}

	bool node::are_octants_empty() {
		for (uint8_t i = 0; i < NUM_OCTANTS; i++) {
			if (_octants[i] != nullptr && !_octants[i]->empty()) {
				return false;
			}
		}
		return true;
	}

	void node::push_object(obj_type object) {
		_objects.push_back(object);
		_time_to_death = OCTREE_LIFESPAN;
	}

	bool node::empty() const {
		if (_objects.size() > 0)
			return false;
		if (_no_children == 0)
			return true;
		
		for (uint8_t i = 0; i < NUM_OCTANTS; i++) {
			if (_octants[i] != nullptr && !_octants[i]->empty()) {
				return false;
			}
		}
		return true;
	}

	bool node::contains(const obj_type& object) const{
		return _bounds.contains(object->bounds);
	}

	bool node::intersects(const obj_type& object) const {
		return _bounds.intersects(object->bounds);
	}

	bool node::can_fit(const obj_type& object) const {
		return _bounds.can_fit(object->bounds);
	}

	aabb node::octant_size() const {
		glm::vec3 half_size = _bounds.size() * 0.5f;
		return aabb(glm::vec3(0), half_size);
	}

}