#pragma once

#include <vector>
#include <tuple>

#include "uiinstance.h"

namespace feng::ui {

	class ui_layer {
	public:
		ui_layer(uint8_t render_order = 0);

		uint8_t render_order;
		bool support_input = false;
		bool is_active = true;

		void start();
		void update();
		uii_sprt add_instance(uimodel* model, glm::vec2 pos_in_ndc = glm::vec3(0), glm::vec2 size = glm::vec2(1));
		std::tuple<uii_sprt, uii_sprt, uii_sprt, uii_sprt> process_input();

	private:
		std::vector<uii_sprt> _instances;
	};

}