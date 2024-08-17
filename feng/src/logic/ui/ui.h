#pragma once

#include <vector>
#include <memory>

#include "../../graphics/uimodel.h"
#include "ui_layer.h"
#include "../../graphics/shader.h"

namespace feng::ui {

	class ui {
	public:
		ui(shader& shader);

		std::shared_ptr<uimodel> create_model(std::vector<mesh2d> meshes);
		std::shared_ptr<ui_layer> create_layer(uint8_t render_order = 0);
		std::shared_ptr<uiinstance> add_instance(
			ui_layer* layer, 
			uimodel* model, 
			glm::vec2 pos_in_ndc = glm::vec3(0), 
			glm::vec2 size = glm::vec2(1));
		std::shared_ptr<uiinstance> add_instance(
			std::shared_ptr<ui_layer> layer, 
			std::shared_ptr<uimodel> model,
			glm::vec2 pos_in_ndc = glm::vec3(0), 
			glm::vec2 size = glm::vec2(1));

		void start();
		void update();
		void render();

	private:
		std::vector<std::shared_ptr<uimodel>> _models;
		std::vector<std::shared_ptr<ui_layer>> _layers;
		shader& _shader;
		glm::mat4 _ortho_proj;

	};

}