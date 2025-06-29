#pragma once

#include <memory>

#include "../../algorithms/aabb.h"

namespace feng {

	class instance;

	class bounds_updater_context {
	public:
		enum class autonomy : uint8_t {
			independent = 0,
			dependent = 1
			//mixed
		};

		bounds_updater_context();

		autonomy get_autonomy() const;
		aabb calculate_bounds() const;

		virtual aabb get_base() const = 0;
		virtual glm::vec3 get_position() const = 0;
		virtual glm::vec3 get_size() const = 0;
		virtual glm::vec3 get_rotation() const = 0;

	protected:
		bounds_updater_context(autonomy aut);

	private:
		autonomy _autonomy;

	};

	class bounds_updater_context_dependent : public bounds_updater_context {
	public:
		bounds_updater_context_dependent();

		aabb get_base() const;

	protected:
		std::weak_ptr<bounds_updater_context> _independent;

		aabb get_independent_bounds() const;
		void find_independent(instance* inst);

	};

}