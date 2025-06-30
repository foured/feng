#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <variant>
#include <optional>

#include "../logging/logging.h"

namespace feng {

	struct edge {
		glm::vec3 p1, p2;

		static edge overlap(const edge& e1, const edge& e2, float epsilon = FLT_EPSILON);
		static bool are_on_same_line(const edge& e1, const edge& e2, float epsilon = FLT_EPSILON);

	};

	struct triangle {

		triangle(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3);
		triangle(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3, glm::vec3 _n);

		glm::vec3 p1;
		glm::vec3 p2;
		glm::vec3 p3;

		glm::vec3 normal;

		bool is_coplanar(const triangle& other) const;
		void calculate_normal();

	};

	struct polygon {
		std::vector<glm::vec3> points;
		glm::vec3 normal;

		bool is_coplanar(const polygon& other) const;
		bool is_coplanar(const glm::vec3& point) const;
		void calculate_normal();

		std::optional<edge> cyrus_beck_clip(const edge& seg, float epsilon = 1e-6f) const;
		std::optional<polygon> sutherland_hodgman_clip(const polygon& clipper);

	};

	enum class collision_contact_type : uint8_t {
		point = 0,
		edge = 1,
		polygon = 2
	};

	class collision_contact {
	public:
		std::variant<polygon, edge, glm::vec3> data;
		collision_contact_type type;

		template<typename T> 
		T get() const {
			if (std::holds_alternative<T>(data)) {
				return std::get<T>(data);
			}
			THROW_ERROR("You are trying to get non-existent type from variant");
		}

		static collision_contact overlap(const collision_contact& c1, const collision_contact& c2);

	private:
		static collision_contact overlap_edge_and_polygon(const collision_contact& ed, const collision_contact& pl);

	};

	struct collision_data {
		//points TO center
		collision_contact contact;
		glm::vec3 axis = glm::vec3(0.0f);
		float penetration = FLT_MAX;

		void invert();
	};
	
	class sphere_collider_base;
	class sat_collider_base;

	class  collider_base {
	public:
		// last collision data
		collision_data lcd;

		virtual void update_collider_data() = 0;
		virtual void check_changes() = 0;

		virtual bool collides(collider_base* other) = 0;
		virtual bool collides_shape(sat_collider_base* other) = 0;
		virtual bool collides_shape(sphere_collider_base* other) = 0;

		bool was_changed_after_update();

	protected:
		bool _was_changed_after_update = false;

		virtual glm::vec2 project_onto(const glm::vec3& axis) const = 0;
		virtual collision_contact calculate_collision_contact(const glm::vec3& axis) const = 0;

		bool check_overlap(const glm::vec2& proj1, const glm::vec2& proj2, const glm::vec3& axis);
		bool check_axis(collider_base* other, const glm::vec3& axis);
		static float calculate_penetration(const glm::vec2& v1, const glm::vec2& v2);

	};

	class sat_collider_base : public collider_base {
	public:
		sat_collider_base(const std::vector<glm::vec3>& points, const std::vector<glm::vec3>& normals);

		bool collides(collider_base* other) override;
		bool collides_shape(sat_collider_base* other) override;
		bool collides_shape(sphere_collider_base* other) override;

		collision_contact calculate_collision_contact(const glm::vec3& axis) const override;

	protected:
		std::vector<glm::vec3> _points;
		std::vector<glm::vec3> _normals;
		glm::vec3 _center;

		void calculate_center();

		glm::vec2 project_onto(const glm::vec3& axis) const override;

	};

	class sphere_collider_base : public collider_base {
	public:
		sphere_collider_base(const glm::vec3& center, float radius);

		bool collides(collider_base* other) override;
		bool collides_shape(sphere_collider_base* other) override;
		bool collides_shape(sat_collider_base* other) override;

		float get_radius() const;
		glm::vec3 get_center() const;

		collision_contact calculate_collision_contact(const glm::vec3& axis) const override;

	protected:
		float _radius;
		glm::vec3 _center;

		glm::vec2 project_onto(const glm::vec3& axis) const override;

	};

}