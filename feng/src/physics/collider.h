#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace feng {

	struct triangle {

		triangle(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3);
		triangle(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3, glm::vec3 _n);

		glm::vec3 p1;
		glm::vec3 p2;
		glm::vec3 p3;

		glm::vec3 normal;

		bool is_coplanar(const triangle* other) const;
		void calculate_normal();

	};

	struct polygon {
		std::vector<glm::vec3> points;
		glm::vec3 normal;

		bool is_coplanar(const polygon* other) const;
		bool is_coplanar(const glm::vec3& point) const;
		void calculate_normal();

	};

	struct collision_data {
		float penetration = FLT_MAX;
		glm::vec3 axis = glm::vec3(0.0f);

		void invert();
	};
	
	class sphere_collider_base;
	class sat_collider_base;

	class  collider_base {
	public:
		virtual void update_collider_data() = 0;
		virtual void check_changes() = 0;

		virtual bool collides(collider_base* other, collision_data* out) = 0;
		virtual bool collides_shape(sat_collider_base* other, collision_data* out) = 0;
		virtual bool collides_shape(sphere_collider_base* other, collision_data* out) = 0;

		bool was_changed_after_update();

	protected:
		bool _was_changed_after_update = false;

	};

	class sat_collider_base : public collider_base {
	public:
		sat_collider_base(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& normals);

		bool collides(collider_base* other, collision_data* out) override;
		bool collides_shape(sat_collider_base* other, collision_data* out) override;
		bool collides_shape(sphere_collider_base* other, collision_data* out) override;

	protected:
		std::vector<glm::vec3> _points;
		std::vector<glm::vec3> _normals;
		glm::vec3 _center;

		void calculate_center();

	private:
		bool check_axis_sat(sat_collider_base* other, const glm::vec3& axis, collision_data* out);
		bool check_axis_sphere(sphere_collider_base* other, const glm::vec3& axis, collision_data* out);
		bool check_overlap(const glm::vec2& proj1, const glm::vec2& proj2, const glm::vec3& axis, collision_data* out);
		glm::vec2 project_onto(const glm::vec3& axis) const;
		float calculate_penetration(const glm::vec2& v1, const glm::vec2& v2) const;

	};

	class sphere_collider_base : public collider_base {
	public:
		sphere_collider_base(const glm::vec3& center, float radius);

		bool collides(collider_base* other, collision_data* out);
		bool collides_shape(sphere_collider_base* other, collision_data* out);
		bool collides_shape(sat_collider_base* other, collision_data* out);

		float get_radius() const;
		glm::vec3 get_center() const;

	protected:
		float _radius;
		glm::vec3 _center;

	};

}