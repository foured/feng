#pragma once

#include <memory>

#include "../component.h"
#include "../../../algorithms/aabb.h"
#include "../../contexts/bounds_updater_context.h"
#include "../../../physics/collision/collider.h"
#include "../../contexts/collision_receiver_context.h"

namespace feng {

	class simple_collider : public component {
	public:
		aabb bounds;

		simple_collider(instance* instance);
		simple_collider(instance* instance, std::weak_ptr<bounds_updater_context> updater_context);
		
		void set_new_context(std::weak_ptr<bounds_updater_context> updater_context);
		bool update_bounds();

		bool context_expired() const;
		bool collider_expired() const;
		bool intersects(std::shared_ptr<simple_collider> target) const;
		bool intersects(simple_collider* target) const;
		void check_collision(std::shared_ptr<simple_collider> other);

		void start() override;
		void late_start() override;
		void update() override;
		std::shared_ptr<component> copy(instance* new_instance);

		void serialize(data::wfile* file) override;
		void deserialize(data::rfile* file, scene* scene) override;

	private:
		std::vector<std::weak_ptr<collision_receiver_context>> _collision_receivers;
		std::weak_ptr<bounds_updater_context> _updater_context;
		std::weak_ptr<phys::collider_base> _collider_base;

		bool search_for_context();
		bool search_for_collider();
		bool search_for_collision_receivers();
		bool update_bounds_forced();

		void trigger_collision_receivers(const advanced_collision_data& data) const;

	};

}