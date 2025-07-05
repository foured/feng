#pragma once

#include <variant>
#include <glm/glm.hpp>

#include "shapes.h"
#include "../../logging/logging.h"

namespace feng::phys {

	enum class collision_contact_type : uint8_t {
		point = 0,
		edge = 1,
		polygon = 2
	};

	class collision_contact {
	public:
		std::variant<polygon, edge, glm::vec3> data;
		collision_contact_type type;

		uint32_t type_to_int() const;

		template<typename T>
		const T* get_pointer() const {
			if (auto r = std::get_if<T>(&data)) {
				return r;
			}
			THROW_ERROR("You are trying to get non-existent type from variant");
		}

		static collision_contact overlap(const collision_contact& c1, const collision_contact& c2);

	private:
		template<typename T>
		T get() const {
			if (std::holds_alternative<T>(data)) {
				return std::get<T>(data);
			}
			THROW_ERROR("You are trying to get non-existent type from variant");
		}

		static collision_contact overlap_edge_and_polygon(const collision_contact& ed, const collision_contact& pl);

	};

	struct collision_data {
		//points TO center
		collision_contact contact;
		glm::vec3 axis = glm::vec3(0.0f);
		float penetration = FLT_MAX;

		void invert();
	};

}