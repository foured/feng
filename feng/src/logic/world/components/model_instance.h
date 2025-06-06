#pragma once

#include "../component.h"
#include "../../../graphics/model.h"

namespace feng {

	class model_instance : public component {
	public:
		model_instance(instance* instance, std::shared_ptr<model> model);

		void start();
		void update();
		std::shared_ptr<component> copy(instance* new_instance);

		void serialize(data::wfile* file) override;
		void deserialize(data::rfile* file, scene* scene) override;

		void render_alone(shader& shader);

		aabb calculate_bounds();

		std::shared_ptr<model> get_model() const;

	private:
		friend class instance;

		std::shared_ptr<model> _model;

	};

}