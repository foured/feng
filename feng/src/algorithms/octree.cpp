#include "octree.h"

#include <optional>

#include "../logic/world/instance.h"
#include "../utilities/utilities.h"

namespace feng::octree {
	
	// OBJECT TYPE -----------------------------------------------------------------------------------------------------
	
	// PUBLIC-----------------------------------------------------------------------------------------------------------
	object_type::object_type(std::shared_ptr<simple_collider> object)
		: _object(object) {
		_static = object->get_instance()->flags.get(INST_FLAG_STATIC);
	}

	bool object_type::is_static() const {
		return _static;
	}

	bool object_type::expired() const {
		return _object.expired();
	}

	bool object_type::is_inserted() const {
		return _inserted;
	}

	void object_type::set_inserted() {
		_inserted = true;
	}

	simple_collider* object_type::get() const {
		return get_sp().get();
	}

	std::shared_ptr<simple_collider> object_type::get_sp() const {
		auto ptr = _object.lock();
		FENG_ASSERT(ptr, "The octree object was destroyed, but someone is trying to work with it.");
		return ptr;
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
		_last_node = hw <= MIN_OCTREE_WIDTH;
	}

	node::~node() {
		for (auto child : _octants) {
			if (child != nullptr) {
				delete child;
				child = nullptr;
			}
		}
	}

	bool node::add_insance(std::shared_ptr<simple_collider> object) {
		return add_insance(new object_type(object));
	}

	bool node::add_insance(object_type* object) {
		if (!contains(object)) {
			return false;
		}
		
		add_contained_instance(object);
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
			
			// processing objects : 
			//		- check if died
			//		- check if moved

			std::vector<object_type*> remaining;
			remaining.reserve(_objects.size());
			bool added_to_child = false;
			for (auto object : _objects) {
				// deid
				if (object->expired()) {
					delete object;
					continue;       
				}

				// didn`t move
				if (object->is_static() || !object->get()->update_bounds()) {
					remaining.push_back(object);
					continue;
				}

				// moved out of bounds
				if (!contains(object)) {
					if (_parent) {
						_left_objects.push_back(object);
					}
					else {
						LOG_WARNING("Object ", object->get()->get_instance_uuid_string(), " moved out of octree bounds");
						delete object;
					}
					continue;
				}
				
				// moved inside bounds
				added_to_child = false;

				// проблема в том, что октанты мертвы, не создаетс€ нужный октант -> объект добавл€етс€ в родител€

				for (auto octant : _octants) {
					if (octant && octant->add_insance(object)) {
						added_to_child = true;
						break;
					}
				}
				if (!added_to_child) {
					remaining.push_back(object);
				}
			}
			_objects = std::move(remaining);

			// processing collisions
			// have to be checked after all objects manipulations
			// 100% not in this function
		}
	}

	void node::process_left_objects() {
		for (uint8_t i = 0; i < NUM_OCTANTS; i++) {
			if (_octants[i] != nullptr) {
				_octants[i]->process_left_objects();
			}
		}

		if (_left_objects.size() > 0) {
			for (auto object : _left_objects) {
				if (!_parent->add_instance_upwards(object)) {
					LOG_WARNING("Object ", object->get()->get_instance_uuid_string(), " moved out of octree bounds");
					delete object;
				}
			}
			_left_objects.clear();
		}
	}

	void node::check_collisions() {
		for (size_t i = 0; i < _objects.size(); ++i) {
			for (size_t j = i + 1; j < _objects.size(); ++j) {
				check_pair_collision(_objects[i], _objects[j]);
			}

			for (auto octant : _octants) {
				if (octant && octant->intersects(_objects[i])) {
					octant->check_against(_objects[i]);
				}
			}
		}

		for (auto octant : _octants) {
			if (octant != nullptr) {
				octant->check_collisions();
			}
		}
	}

	uint32_t node::find_instance(std::shared_ptr<simple_collider> instance) {
		uint32_t count = 0;
		for (auto* object : _objects) {
			if (object->get_sp() == instance) {
				count += 1;
			}
		}

		for (auto octant : _octants) {
			if (octant != nullptr) {
				count += octant->find_instance(instance);
			}
		}
		return count;
	}

	void node::delete_unused_children() {
		for (auto octant : _octants) {
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

	void node::check_against(const std::vector<object_type*>& strangers) {
		for (auto stranger : strangers) {
			for (auto local : _objects) {
				check_pair_collision(stranger, local);
			}
		}

		for (auto octant : _octants) {
			if (octant != nullptr) {
				octant->check_against(strangers);
			}
		}
	}

	void node::check_against(object_type* stranger) {
		for (auto local : _objects) {
			check_pair_collision(local, stranger);

		}
		for (auto octant : _octants) {
			if (octant != nullptr && octant->intersects(stranger)) {
				octant->check_against(stranger);
			}
		}
	}

	void node::on_collision(object_type* object1, object_type* object2) {
		//LOG_INFO("Intersection!");
		object1->get()->check_collision(object2->get_sp());
	}

	void node::check_pair_collision(object_type* object1, object_type* object2) {
		if (object1->is_static() && object2->is_static()) {
			// if both static, no reason to check
			return;
		}

		if (object1->get()->bounds.intersects(object2->get()->bounds)) {
			on_collision(object1, object2);
		}
		//if (object1->is_static()) {
		//	check_pair_collision_ds(object2, object1);
		//}
		//else {
		//	check_pair_collision_ds(object1, object2);
		//}
	}

	void node::check_pair_collision_ds(object_type* d, object_type* s) {
		std::optional<aabb> static_intersection = _bounds.intersect_safe(s->get()->bounds);
		if (static_intersection) {
			if (d->get()->bounds.intersects(static_intersection.value())) {
				on_collision(d, s);
			}
		}
	}

	bool node::add_instance_upwards(object_type* object) {
		if (!contains(object)) {
			if (!_parent) {
				return false;
			}
			return _parent->add_instance_upwards(object);
		}

		add_contained_instance(object);

		return true;
	}

	void node::add_contained_instance(object_type* object) {
		push_object(object);

		if (_last_node || (_objects.size() <= OCTREE_POPULATION_LIMIT && are_octants_empty())) {
			return;
		}

		revive_dead_octants();

		std::vector<object_type*> remaining;
		bool inserted;
		for (auto obj : _objects) {
			if (obj->is_inserted()) {
				remaining.push_back(obj);
				continue;
			}
			inserted = false;
			for (auto child : _octants) {
				if (child->add_insance(obj)) {
					inserted = true;
					break;
				}
			}
			if (!inserted) {
				remaining.push_back(obj);
			}
		}

		_objects = std::move(remaining);

		// static objects separation
		// пока не пон€л, что делать с повторением коллизии между не статическим объектом и част€ми статического

		//if (remaining.size() > 0) {
		//	// check if can be stored in children
		//	std::vector<node*> intersections;
		//	for (auto object : remaining) {
		//		if (!object->is_static() || object->is_inserted()) {
		//			_objects.push_back(object);
		//			continue;
		//		}
		//		// insert to intersections
		//		for (auto octant : _octants) {
		//			if (octant->intersects(object)) {
		//				intersections.push_back(octant);
		//			}
		//		}

		//		if (intersections.empty()) {
		//			THROW_ERROR("Object stands in octant but doesnt intersect with its children");
		//		}

		//		if (intersections.size() < NUM_OCTANTS) {
		//			for (auto octant : intersections) {
		//				//octant->push_object(object);
		//				octant->add_to_optimal_intersecting_node(object);
		//			}
		//		}
		//		else {
		//			_objects.push_back(object);
		//		}
		//		object->set_inserted();
		//		intersections.clear();
		//	}
		//}
	}


	void node::add_to_optimal_intersecting_node(object_type* object) {
		// _no_children == 0 || _last_node || 
		if (!can_fit(object) || _last_node || _no_children == 0) {
			push_object(object);
			return;
		}

		revive_dead_octants();

		for (const auto octant : _octants)
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

	void node::push_object(object_type* object) {
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

	bool node::contains(object_type* object) const{
		return _bounds.contains(object->get()->bounds);
	}

	bool node::intersects(object_type* object) const {
		return _bounds.intersects(object->get()->bounds);
	}

	bool node::can_fit(object_type* object) const {
		return _bounds.can_fit(object->get()->bounds);
	}

	aabb node::octant_size() const {
		glm::vec3 half_size = _bounds.size() * 0.5f;
		return aabb(glm::vec3(0), half_size);
	}

}