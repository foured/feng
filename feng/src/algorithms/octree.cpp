#include "octree.h"

#include "../logic/world/instance.h"

#include "../utilities/utilities.h"

namespace feng::octree {
	
	// PUBLIC-------------------------------------------------------------------------------------------------------------
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
			delete child;
			child = nullptr;
		}
	}

	bool node::add_insance(obj_type object) {
		if (!contains(object))
			return false;
		add_instance_good_conditions(object);
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

		// processing moving objects
		std::vector<obj_type> remaining;
		for (obj_type& object : _objects) {
			if (object->update_bounds() && !contains(object) 
				&& _parent) {
				if (!_parent->try_add_instance_upward(object))
					LOG_WARNING("Object moved out from octree ", object->get_instance()->get_uuid_string());
			}
			else {
				remaining.push_back(object);
			}
		}
		_objects = std::move(remaining);

		// processing collisions
		if (_objects.size() >= 2) {
			for (size_t i = 0; i < _objects.size() - 1; i++) {
				for (size_t j = i + 1; j < _objects.size(); j++) {
					if (_objects[i]->intersects(_objects[j])) {
						std::string o1 = _objects[i]->get_instance()->get_uuid_string();
						std::string o2 = _objects[j]->get_instance()->get_uuid_string();
						LOG_INFO("Objects intersecting: ", o1, " ", o2);
					}
				}
			}
		}
	}

	// PRIVATE-------------------------------------------------------------------------------------------------------------

	node::node(glm::vec3 pos, float width, node* parent)
		: node(pos, width) {
		_parent = parent;
		_root = _parent == nullptr;
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
		case octants::RBB: offset = { quarter, -quarter, -quarter }; break;  // Right Bottom Back
		case octants::RBF: offset = { quarter, -quarter,  quarter }; break;  // Right Bottom Front
		case octants::RTB: offset = { quarter,  quarter, -quarter }; break;  // Right Top    Back
		case octants::RTF: offset = { quarter,  quarter,  quarter }; break;  // Right Top    Front
		}

		glm::vec3 new_center = center + offset;
		return new node(new_center, half_size, this);
	}

	bool node::try_add_instance_upward(obj_type object) {
		if (!contains(object)) {
			if (_parent)
				return _parent->try_add_instance_upward(object);
			else
				return false;
		}
		add_instance_good_conditions(object);
		return true;
	}

	void node::add_instance_good_conditions(obj_type object) {
		_objects.push_back(object);
		_time_to_death = OCTREE_LIFESPAN;

		if (_last_node || _objects.size() <= OCTREE_POPULATION_LIMIT)
			return;

		for (size_t i = 0; i < NUM_OCTANTS; i++) {
			if (_octants[i] == nullptr) {
				_octants[i] = generate_octant(static_cast<octants>(i));
				_no_children += 1;
			}
		}

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

		_objects = std::move(remaining);
	}

	bool node::contains(obj_type object) const{
		return _bounds.contains(object->bounds);
	}

}