#pragma once

#include "../component.h"
#include "../../../graphics/model.h"
#include "../../contexts/bounds_updater_context.h"

namespace feng {

	class model_instance : public component, public bounds_updater_context {
	public:
		model_instance(instance* instance, std::shared_ptr<model> model);

		// Default
		void start();
		void update();
		std::shared_ptr<component> copy(instance* new_instance);

		// Overrides
		void serialize(data::wfile* file) override;
		void deserialize(data::rfile* file, scene* scene) override;
		aabb calculate_bounds() const override;

		// Specific
		void render_alone(shader& shader);
		std::shared_ptr<model> get_model() const;

	private:
		friend class instance;

		std::shared_ptr<model> _model;

	};

}