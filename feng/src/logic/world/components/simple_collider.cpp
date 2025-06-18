#include "simple_collider.h"

#include "../instance.h"
#include "../../data_management/files.h"
#include "../scene.h"

namespace feng {

	simple_collider::simple_collider(instance* instance) : component(instance) {
		if (!search_for_context_and_set()) {
			LOG_WARNING("Can`t find bounds_updater_context in ", _instance->get_uuid_string());
		}
		else {
			update_bounds_forced();
		}
	}

	simple_collider::simple_collider(instance* instance, std::weak_ptr<bounds_updater_context> updater_context)
		: component(instance), _updater_context(updater_context) {
		update_bounds();
	}

	void simple_collider::set_new_context(std::weak_ptr<bounds_updater_context> updater_context) {
		_updater_context = updater_context;
	}

	bool simple_collider::update_bounds() {
		if (!_instance->transform.changed_this_frame)
			return false;

		return update_bounds_forced();
	}


	bool simple_collider::context_expired() const {
		return _updater_context.expired();
	}

	bool simple_collider::intersects(std::shared_ptr<simple_collider> target) const {
		return bounds.intersects(target->bounds);
	}

	void simple_collider::start() {
		if (context_expired() && !search_for_context_and_set())
			THROW_ERROR("Instance ", _instance->get_uuid_string(), " has no bounds_updater_context");
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

	bool simple_collider::search_for_context_and_set() {
		std::shared_ptr<bounds_updater_context> buc = _instance->try_find_component_of_type<bounds_updater_context>();
		if (buc) {
			_updater_context = buc;
			return true;
		}
		return false;
	}

	bool simple_collider::update_bounds_forced() {
		auto context = _updater_context.lock();
		if (!context && !search_for_context_and_set())
		{
			LOG_WARNING("bounds_updater_context expired in ", _instance->get_uuid_string());
			return false;
		}

		bounds = _updater_context.lock()->calculate_bounds();
		return true;
	}

}