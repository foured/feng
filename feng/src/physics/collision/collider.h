#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <variant>
#include <optional>

#include "algorithms/gjk.h"
#include "algorithms/sat.h"
#include "collision_data.h"
#include "colliders_data.h"

namespace feng::phys {
	
	class sphere_collider_base;
	class mesh_collider_base;

	class  collider_base : protected gjk::collider, virtual public sat::collider,
		virtual protected base_collider_data {
	public:
		bool was_changed_after_update();

		virtual void update_collider_data() = 0;
		virtual void check_changes() = 0;
		virtual bool is_static() const = 0;
		virtual void add_position(const glm::vec3& offset) = 0;

	protected:
		friend class physics;

		bool _was_changed_after_update = false;

		virtual collision_contact calculate_collision_contact(const glm::vec3& axis) const = 0;
		virtual void add_data_offset(const glm::vec3& offset) = 0;

	};

	class mesh_collider_base : public collider_base, public sat::mesh_collider, virtual public mesh_collider_data {
	public:
		mesh_collider_base(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& normals);

		collision_contact calculate_collision_contact(const glm::vec3& axis) const override;

		// GJK
		glm::vec3 find_furthest_point(const glm::vec3& axis) const override;

	protected:
		void calculate_center();

		void add_data_offset(const glm::vec3& offset) override;

	};

	class sphere_collider_base : public collider_base, public sat::sphere_collider, virtual public sphere_collider_data {
	public:
		sphere_collider_base(const glm::vec3& center, float radius);

		float get_radius() const;
		glm::vec3 get_center() const;

		collision_contact calculate_collision_contact(const glm::vec3& axis) const override;

		// GJK
		glm::vec3 find_furthest_point(const glm::vec3& axis) const override;
		
	protected:
		void add_data_offset(const glm::vec3& offset) override;

	};

}