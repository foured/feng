#pragma once

#include "collision/collider.h"

namespace feng::phys {

	class physics {
	public:
		static bool collision(collider_base* col_1, collider_base* col_2, collision_data* data);

	private:
		static void orient_axis(collider_base* col_1, collider_base* col_2, collision_data* data);

	};

}