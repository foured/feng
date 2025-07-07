#pragma once

#include "../../physics/collision/collider.h"
#include "../world/instance.h"

namespace feng {

	struct advanced_collision_data {
		instance* other;
		phys::collision_data* collision_data;
	};

	class collision_receiver_context {
	public:
		virtual ~collision_receiver_context() = default;

		virtual void on_collision(const advanced_collision_data& data) = 0;
	};

}