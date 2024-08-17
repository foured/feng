#pragma once

#include "model2d_renderer.h"
#include "../logic/ui/uiinstance.h"
#include "../logic/ui/uibounds.h"

namespace feng::ui {
	class ui_layer;
}

namespace feng {

	class uimodel : public model2d_renderer {
	public:
		uimodel(std::vector<mesh2d> meshes);

		void render(shader& uishader);
		std::shared_ptr<ui::uiinstance> add_instance(ui::ui_layer* layer, glm::vec2 pos_in_ndc = glm::vec3(0), glm::vec2 size = glm::vec2(1));

		ui::uibounds_rect rect();

	private:	
		std::vector<glm::vec3> _positions;
		std::vector<glm::vec2> _sizes;
		arraybuffer _pos_array_buffer;
		arraybuffer _size_array_buffer;

		std::vector<std::shared_ptr<ui::uiinstance>> _instances;
		ui::uibounds_rect _rect;

		void setup();
		void _gen_arrays_data();

	};

}