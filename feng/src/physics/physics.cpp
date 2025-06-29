#include "physics.h"

namespace feng {

	float physics::gravity = -9.81f;
	glm::vec3 physics::gravity_vec = glm::vec3(0, physics::gravity, 0);

}