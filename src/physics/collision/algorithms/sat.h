#pragma once

#include "../collision_data.h"
#include "../colliders_data.h"

namespace feng::phys::sat {

	struct mesh_collider;
	struct sphere_collider;

	class collider {
	protected:
		friend class solver;
		friend class mesh_collider;
		friend class sphere_collider;

		virtual bool collides(const collider* other, collision_data* data) const = 0;
		virtual bool collides_shape(const mesh_collider* other, collision_data* data) const = 0;
		virtual bool collides_shape(const sphere_collider* other, collision_data* data) const = 0;

		virtual glm::vec2 project_onto(const glm::vec3& axis) const = 0;
	};

	class mesh_collider : virtual protected collider, virtual protected mesh_collider_data {
	protected:
		friend class solver;

		bool collides(const collider* other, collision_data* data) const override;
		bool collides_shape(const mesh_collider* other, collision_data* data) const override;
		bool collides_shape(const sphere_collider* other, collision_data* data) const override;

		glm::vec2 project_onto(const glm::vec3& axis) const override;

		bool chech_axes(const collider* other, collision_data* data) const;
	};

	class sphere_collider : virtual protected collider, virtual protected sphere_collider_data {
	protected:
		friend class solver;

		bool collides(const collider* other, collision_data* data) const override;
		bool collides_shape(const mesh_collider* other, collision_data* data) const override;
		bool collides_shape(const sphere_collider* other, collision_data* data) const override;

		glm::vec2 project_onto(const glm::vec3& axis) const override;
	};

	struct solver {
		static bool solve(collider* col_1, collider* col_2, collision_data* data);
		static bool solve(const mesh_collider* col_1, const mesh_collider* col_2, collision_data* data);
		static bool solve(const mesh_collider* mesh, const sphere_collider* sphere, collision_data* data);
		static bool solve(const sphere_collider* col_1, const sphere_collider* col_2, collision_data* data);

		static bool check_axis(
			const collider* col_1, const collider* col_2,
			const glm::vec3& axis, collision_data* data);

	};


}