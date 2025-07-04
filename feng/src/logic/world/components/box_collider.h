#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../component.h"
#include "../../../physics/collider.h"
#include "../../contexts/collision_receiver_context.h"
#include "../../contexts/bounds_updater_context.h"

namespace feng {

	class box_collider : public component, public phys::mesh_collider_base, public collision_receiver_context,
		public bounds_updater_context_dependent {
	public:
		//box_collider(instance* instance,
		//	const glm::vec3& offset = glm::vec3(0),
		//	const glm::vec3& size = glm::vec3(0),
		//	const glm::mat3& rotation_mat = glm::identity<glm::mat3>());

		box_collider(instance* instance, 
			const glm::vec3& offset = glm::vec3(0), 
			const glm::vec3& size = glm::vec3(1),
			const glm::vec3& rotation = glm::vec3(0));

		void start() override;
		void late_start() override;
		void update() override;
		std::shared_ptr<component> copy(instance* new_instance);

		// overrides
		void update_collider_data() override;
		void check_changes() override;
		bool is_static() const override;
		void add_position(const glm::vec3& offset) override;
		
		void on_collision(const advanced_collision_data& data) override;

		glm::vec3 get_position() const override;
		glm::vec3 get_size() const override;
		glm::vec3 get_rotation() const override;

		void serialize(data::wfile* file) override;
		void deserialize(data::rfile* file, scene* scene) override;

	private:
		glm::vec3 _offset;
		glm::vec3 _size; 
		glm::vec3 _rotation;

	};

}