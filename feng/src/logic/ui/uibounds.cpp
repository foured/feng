#include "uibounds.h"

namespace feng::ui {

	uibounds_rect::uibounds_rect(glm::vec2 min, glm::vec2 max) {
		b_min = min;
		b_max = max;
	}

	uibounds_rect::uibounds_rect(std::vector<mesh2d>& meshes) {
		b_min = meshes[0].get_vertices()[0].position;
		b_max = meshes[0].get_vertices()[0].position;

		for (mesh2d& mesh : meshes) {
			std::vector<vertex2d> vrtcs = mesh.get_vertices();
			for (vertex2d& vertex : vrtcs) {
				if (vertex.position.x < b_min.x)
					b_min.x = vertex.position.x;
				if (vertex.position.y < b_min.y)
					b_min.y = vertex.position.y;

				if (vertex.position.x > b_max.x)
					b_max.x = vertex.position.x;
				if (vertex.position.y > b_max.y)
					b_max.y = vertex.position.y;
			}
		}
	}

	bool uibounds_rect::is_point_in_rect(glm::vec2 point, uibounds_rect& rect) {
		return point.x <= rect.b_max.x && point.x >= rect.b_min.x
			&& point.y <= rect.b_max.y && point.y >= rect.b_min.y;
	}

	bool uibounds_rect::is_point_in_rect(glm::vec2 point, glm::vec2 min, glm::vec2 max) {
		return point.x <= max.x && point.x >= min.x
			&& point.y <= max.y && point.y >= min.y;
	}

	// uibounds

	uibounds::uibounds(uibounds_rect rect, uitransform* transform)
		: _rect(rect), _transform(transform) {}

	bool uibounds::intersects_with(uibounds& bounds) {
		uibounds_rect tr = bounds.real_rect();
		uibounds_rect cr = real_rect();

		return uibounds_rect::is_point_in_rect({ tr.b_min }, cr)
			|| uibounds_rect::is_point_in_rect({ tr.b_min.x, tr.b_max.y }, cr)
			|| uibounds_rect::is_point_in_rect({ tr.b_max }, cr)
			|| uibounds_rect::is_point_in_rect({ tr.b_max.x, tr.b_min.y }, cr);
	}

	bool uibounds::intersects_with(glm::vec2 point) {
		uibounds_rect cr = real_rect();
		return uibounds_rect::is_point_in_rect(point, cr);
	}

	glm::vec2 uibounds::apply_transform(glm::vec2 target) {
		return target * _transform->get_size_ndc() + _transform->get_pos_ndc();
	}

	uibounds_rect uibounds::real_rect() {
		return { apply_transform(_rect.b_min), apply_transform(_rect.b_max) };
	}

	uibounds_rect uibounds::rect() {
		return _rect;
	}

}