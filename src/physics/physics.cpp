#include "physics.h"

#include "collision/algorithms/epa.h"
#include "collision/algorithms/sat.h"

namespace feng::phys {

	bool physics::collision(collider_base* col_1, collider_base* col_2, collision_data* data) {
		//if (first->collides(second)) {
		//	bool is_first_static = first->is_static();
		//	bool is_second_static = second->is_static();
		//	if (is_first_static && is_second_static) {
		//		LOG_WARNING("Only static objects detected in collision checks");
		//		return true;
		//	}

		//	if (is_first_static && !is_second_static) {
		//		glm::vec3 offset = second->lcd.axis * second->lcd.penetration;
		//		second->add_position(offset);
		//		second->add_data_offset(offset);
		//	}
		//	else if (!is_first_static && is_second_static) {
		//		glm::vec3 offset = first->lcd.axis * first->lcd.penetration;
		//		first->add_position(offset);
		//		first->add_data_offset(offset);
		//	}
		//	else {
		//		glm::vec3 offset = first->lcd.axis * first->lcd.penetration * 0.5f;
		//		first->add_position(offset);
		//		first->add_data_offset(offset);
		//		second->add_position(-1.0f * offset);
		//		second->add_data_offset(offset);
		//	}

		//	collision_contact c1 = first->calculate_collision_contact();
		//	collision_contact c2 = second->calculate_collision_contact();
		//	collision_contact c3 = collision_contact::overlap(c1, c2);

		//	first->lcd.contact = c3;
		//	second->lcd.contact = c3;
		//	return true;
		//}
		//return false;

		gjk::simplex out;

		if (sat::solver::solve(col_1, col_2, data)) {
		//if (gjk::gjk(col_1, col_2, &out)) {
			//epa::epa(&out, col_1, col_2, data);
			bool is_first_static = col_1->is_static();
			bool is_second_static = col_2->is_static();
			if (is_first_static && is_second_static) {
				LOG_WARNING("Only static objects detected in collision checks");
				return true;
			}

			orient_axis(col_1, col_2, data);

			glm::vec3 axis = data->axis;
			float penetration = data->penetration + 0.0001f;
			glm::vec3 dir = axis * penetration;
			if (is_first_static && !is_second_static) {
				glm::vec3 offset = dir;
				col_2->add_position(offset);
				col_2->add_data_offset(offset);
			}
			else if (!is_first_static && is_second_static) {
				glm::vec3 offset = dir;
				col_1->add_position(offset);
				col_1->add_data_offset(offset);
			}
			else {
				glm::vec3 offset = dir * 0.5f;
				col_1->add_position(offset);
				col_1->add_data_offset(offset);
				col_2->add_position(-1.0f * offset);
				col_2->add_data_offset(offset);
			}
		}
	}

	void physics::orient_axis(collider_base* col_1, collider_base* col_2, collision_data* data) {
		glm::vec3 dir = col_2->_center - col_1->_center;
		if (glm::dot(dir, data->axis) < 0) {
			data->axis *= -1;
		}
	}

}