#pragma once

#include "../component.h"
#include "../../../graphics/model.h"
#include "../../contexts/bounds_updater_context.h"

namespace feng {

	class model_instance : public component, public bounds_updater_context {
	public:
		model_instance(instance* instance, std::shared_ptr<model> model);

		// Default
		void start() override;
		void late_start() override;
		void update() override;
		std::shared_ptr<component> copy(instance* new_instance) override;

		// Overrides
		void serialize(data::wfile* file) override;
		void deserialize(data::rfile* file, scene* scene) override;
		
		aabb get_base() const override;
		glm::vec3 get_position() const override;
		glm::vec3 get_size() const override;
		glm::vec3 get_rotation() const override;

		// Specific
		void render_alone(shader& shader);
		std::shared_ptr<model> get_model() const;

	private:
		friend class instance;

		std::shared_ptr<model> _model;

	};

}