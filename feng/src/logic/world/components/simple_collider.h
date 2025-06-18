#pragma once

#include <memory>

#include "../component.h"
#include "../../aabb.h"
#include "../../contexts/bounds_updater.h"

namespace feng {

	class simple_collider : public component {
	public:
		aabb bounds;

		simple_collider(instance* instance);
		simple_collider(instance* instance, std::weak_ptr<bounds_updater_context> updater_context);
		
		void set_new_context(std::weak_ptr<bounds_updater_context> updater_context);
		bool update_bounds();

		bool context_expired() const;

		void start();
		void update();
		std::shared_ptr<component> copy(instance* new_instance);

		void serialize(data::wfile* file) override;
		void deserialize(data::rfile* file, scene* scene) override;

	private:
		std::weak_ptr<bounds_updater_context> _updater_context;

		bool search_for_context_and_set();

	};

}