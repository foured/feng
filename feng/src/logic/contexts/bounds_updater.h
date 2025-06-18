#pragma once

#include "../aabb.h"

namespace feng {

	class bounds_updater_context {
	public:
		virtual aabb calculate_bounds() const = 0;
	};

}