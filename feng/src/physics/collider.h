#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <variant>
#include <optional>

#include "gjk.h"
#include "collision_data.h"

namespace feng::phys {
	
	class sphere_collider_base;
	class mesh_collider_base;

	class  collider_base : public gjk::collider {
	public:
		// last collision data
		//collision_data lcd;

		virtual void update_collider_data() = 0;
		virtual void check_changes() = 0;

		virtual bool collides(collider_base* other, collision_data* data) = 0;
		virtual bool collides_shape(mesh_collider_base* other, collision_data* data) = 0;
		virtual bool collides_shape(sphere_collider_base* other, collision_data* data) = 0;

		virtual bool is_static() const = 0;
		virtual void add_position(const glm::vec3& offset) = 0;

		bool was_changed_after_update();

	protected:
		friend class physics;

		glm::vec3 _center;
		bool _was_changed_after_update = false;

		virtual glm::vec2 project_onto(const glm::vec3& axis) const = 0;
		virtual collision_contact calculate_collision_contact(const glm::vec3& axis) const = 0;

		virtual void add_data_offset(const glm::vec3& offset) = 0;

		bool check_overlap(const glm::vec2& proj1, const glm::vec2& proj2, const glm::vec3& axis, collision_data* data);
		bool check_axis(collider_base* other, const glm::vec3& axis, collision_data* data);

		static float calculate_penetration(const glm::vec2& v1, const glm::vec2& v2);

	};

	class mesh_collider_base : public collider_base {
	public:
		mesh_collider_base(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& normals);

		bool collides(collider_base* other, collision_data* data) override;
		bool collides_shape(mesh_collider_base* other, collision_data* data) override;
		bool collides_shape(sphere_collider_base* other, collision_data* data) override;

		collision_contact calculate_collision_contact(const glm::vec3& axis) const override;

		// GJK
		glm::vec3 find_furthest_point(const glm::vec3& axis) const override;

	protected:
		std::vector<glm::vec3> _points;
		std::vector<glm::vec3> _normals;

		void calculate_center();

		glm::vec2 project_onto(const glm::vec3& axis) const override;
		void add_data_offset(const glm::vec3& offset) override;

	};

	class sphere_collider_base : public collider_base {
	public:
		sphere_collider_base(const glm::vec3& center, float radius);

		bool collides(collider_base* other, collision_data* data) override;
		bool collides_shape(sphere_collider_base* other, collision_data* data) override;
		bool collides_shape(mesh_collider_base* other, collision_data* data) override;

		float get_radius() const;
		glm::vec3 get_center() const;

		collision_contact calculate_collision_contact(const glm::vec3& axis) const override;

		// GJK
		glm::vec3 find_furthest_point(const glm::vec3& axis) const override;

	protected:
		float _radius;

		glm::vec2 project_onto(const glm::vec3& axis) const override;
		void add_data_offset(const glm::vec3& offset) override;

	};

}