#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../component.h"
#include "../../../physics/collider.h"

namespace feng {

	class box_collider : public component, public sat_collider_base {
	public:
		//box_collider(instance* instance,
		//	const glm::vec3& offset = glm::vec3(0),
		//	const glm::vec3& size = glm::vec3(0),
		//	const glm::mat3& rotation_mat = glm::identity<glm::mat3>());

		box_collider(instance* instance, 
			const glm::vec3& offset = glm::vec3(0), 
			const glm::vec3& size = glm::vec3(1),
			const glm::vec3& rotation = glm::vec3(0));

		void start();
		void update();
		std::shared_ptr<component> copy(instance* new_instance);

		void update_collider_data() override;
		void check_changes() override;

		void serialize(data::wfile* file) override;
		void deserialize(data::rfile* file, scene* scene) override;

	private:
		glm::vec3 _offset;
		glm::vec3 _size; 
		glm::vec3 _rotation;

	};

}