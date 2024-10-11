#include "lights.h"

#include <glm/gtc/matrix_transform.hpp>

namespace feng {

	glm::mat4 DirLight::generate_lightspace_matrix() {
		float border = 5;
		float dlm_near_plane = 0.0f, dlm_far_plane = 17.5f;
		glm::mat4 light_projection = glm::ortho(-border, border, -border, border, dlm_near_plane, dlm_far_plane);
		glm::vec3 pos = -2.0f * direction;
		glm::mat4 light_view = glm::lookAt(
			pos,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

		return light_projection * light_view;
	}

}