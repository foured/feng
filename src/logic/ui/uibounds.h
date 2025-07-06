#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "uitransform.h"
#include "../../graphics/mesh2d.h"

namespace feng::ui {

	struct uibounds_rect {
		uibounds_rect(glm::vec2 min, glm::vec2 max);
		uibounds_rect(std::vector<mesh2d>& meshes);

		static bool is_point_in_rect(glm::vec2 point, uibounds_rect& rect);
		static bool is_point_in_rect(glm::vec2 point, glm::vec2 min, glm::vec2 max);

		glm::vec2 b_min, b_max;
	};

	class uibounds {
	public:
		uibounds(uibounds_rect rect, uitransform* transform);

		bool intersects_with(uibounds& bounds);
		bool intersects_with(glm::vec2 point);

		uibounds_rect real_rect();
		uibounds_rect rect();

	private:
		uibounds_rect _rect;
		uitransform* _transform;

		glm::vec2 apply_transform(glm::vec2 target);

	};


}