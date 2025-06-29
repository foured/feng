#pragma once

#include <glm/glm.hpp>

#include "../component.h"
#include "../../contexts/collision_receiver_context.h"

namespace feng {

	class rigidbody : public component, public collision_receiver_context {
	public:
		rigidbody(instance* instance, float mass);
		
		void start();
		void update();
		std::shared_ptr<component> copy(instance* new_instance);

		void add_force(const glm::vec3& force);

		void on_collision(const advanced_collision_data& data) override;

		void serialize(data::wfile* file) override;
		void deserialize(data::rfile* file, scene* scene) override;

	private:
		glm::vec3 _acceleration = glm::vec3(0.0f);
		glm::vec3 _velocity = glm::vec3(0.0f);
		glm::vec3 _accumulated_force = glm::vec3(0.0f);
		float _mass;

		glm::vec3 get_pos();
		void set_pos(const glm::vec3& pos);

	};

}