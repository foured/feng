#pragma once

#include <memory>
#include <vector>

#include "../../graphics/model.h"
#include "../../graphics/shader.h"

namespace feng {

	class scene {
		void register_model(std::shared_ptr<model> model);
		void render_models(shader& shader);

	private:
		std::vector<std::shared_ptr<model>> _models;

	};

}