#include "simple_collider.h"

#include <ranges>

#include "../instance.h"
#include "../../data_management/files.h"
#include "../scene.h"
#include "../../../physics/physics.h"

namespace feng {

	simple_collider::simple_collider(instance* instance) : component(instance) {
		//if (!search_for_context()) {
		//	LOG_WARNING("Can`t find bounds_updater_context in ", _instance->get_uuid_string());
		//}
		//else {
		//	update_bounds_forced();
		//}
	}

	simple_collider::simple_collider(instance* instance, std::weak_ptr<bounds_updater_context> updater_context)
		: component(instance), _updater_context(updater_context) {
		update_bounds();
	}

	void simple_collider::set_new_context(std::weak_ptr<bounds_updater_context> updater_context) {
		_updater_context = updater_context;
	}

	bool simple_collider::update_bounds() {
		if (!_instance->transform.changed_this_frame) {
			return false;
		}

		if (!collider_expired()) {
			_collider_base.lock()->check_changes();
		}

		return update_bounds_forced();
	}


	bool simple_collider::context_expired() const {
		return _updater_context.expired();
	}

	bool simple_collider::collider_expired() const {
		return _collider_base.expired();
	}

	bool simple_collider::intersects(std::shared_ptr<simple_collider> target) const {
		return intersects(target.get());
	}

	bool simple_collider::intersects(simple_collider* target) const {
		return bounds.intersects(target->bounds);
	}

	void simple_collider::check_collision(std::shared_ptr<simple_collider> other_sc) {
		if (collider_expired() || other_sc->collider_expired()) {
			return;
		}

		std::shared_ptr<phys::collider_base> self = _collider_base.lock();
		std::shared_ptr<phys::collider_base> other = other_sc->_collider_base.lock();

		if (self->was_changed_after_update()) {
			self->update_collider_data();
		}
		if (other->was_changed_after_update()) {
			other->update_collider_data();
		}

		phys::collision_data data;
		if (phys::physics::collision(self.get(), other.get(), &data)) {
			LOG_INFO("Collision");
			//LOG_INFO("Collision axis: ", other->lcd.axis, " contact type: ", self->lcd.contact.type_to_int());
			//glm::vec3 point = *self->lcd.contact.get_pointer<glm::vec3>();
			//LOG_INFO("Point: ", point);
			trigger_collision_receivers({ other_sc->get_instance(), &data });
			data.invert();
			other_sc->trigger_collision_receivers({ get_instance(), &data });
		}

	}

	void simple_collider::start() {
		if (context_expired() && !search_for_context()) {
			THROW_ERROR("Instance ", _instance->get_uuid_string(), " has no bounds_updater_context");
		}

		search_for_collider();
		bool found_receivers = search_for_collision_receivers();
		if (collider_expired() && found_receivers) {
			THROW_ERROR("Collision receivers were found but collider wasn`t.");
		}
	}

	void simple_collider::late_start() {
		update_bounds_forced();
	}

	void simple_collider::update() {

	}

	std::shared_ptr<component> simple_collider::copy(instance* new_instance) {
		return std::make_shared<simple_collider>(new_instance);
	}

	void simple_collider::serialize(data::wfile* file)
	{
		THROW_ERROR("Unimplemented code error.");
	}

	void simple_collider::deserialize(data::rfile* file, scene* scene)
	{
		THROW_ERROR("Unimplemented code error.");
	}

	// PRIVATE ---------------------------------------------------------------------------------------------------------

	bool simple_collider::search_for_context() {
		//std::shared_ptr<bounds_updater_context> buc = _instance->try_find_component_of_type<bounds_updater_context>();
		//if (buc) {
		//	_updater_context = buc;
		//	return true;
		//}
		//return false;
		using spc = std::shared_ptr<bounds_updater_context>;
		std::vector<spc> contexts = _instance->try_find_components_of_type<bounds_updater_context>();

		if (contexts.empty()) {
			return false;
		}

		auto dependent = contexts | std::views::filter([](spc c) {
				return c->get_autonomy() == bounds_updater_context::autonomy::dependent; 
			});
		std::vector<spc> filtered(dependent.begin(), dependent.end());

		if (filtered.size() == 1) {
			_updater_context = filtered[0];
			return true;
		}

		if (filtered.size() > 1) {
			_updater_context = filtered[0];
			LOG_WARNING("More than one dependent bounds_updater_context were found.");
			return true;
		}

		auto independent = contexts | std::views::filter([](spc c) {
				return c->get_autonomy() == bounds_updater_context::autonomy::independent;
			});
		filtered = std::vector<spc>(independent.begin(), independent.end());

		if (filtered.size() == 1) {
			_updater_context = filtered[0];
			return true;
		}

		if (filtered.size() > 1) {
			_updater_context = filtered[0];
			LOG_WARNING("More than one independent bounds_updater_context were found.");
			return true;
		}
		return false;
	}

	bool simple_collider::search_for_collider() {
		std::shared_ptr<phys::collider_base> col = _instance->try_find_component_of_type<phys::collider_base>();
		if (col) {
			_collider_base = col;
			return true;
		}
		return false;
	}

	bool simple_collider::search_for_collision_receivers() {
		_collision_receivers = _instance->
			try_find_components_of_type<collision_receiver_context, std::weak_ptr<collision_receiver_context>>();
		return !_collision_receivers.empty();
	}

	bool simple_collider::update_bounds_forced() {
		auto context = _updater_context.lock();
		if (!context && !search_for_context())
		{
			LOG_WARNING("bounds_updater_context expired in ", _instance->get_uuid_string());
			return false;
		}
		bounds = _updater_context.lock()->calculate_bounds();
		return true;
	}

	void simple_collider::trigger_collision_receivers(const advanced_collision_data& data) const {
		for (const auto& receiver : _collision_receivers) {
			if (!receiver.expired()) {
				receiver.lock()->on_collision(data);
			}
		}
	}

}