#include "ui_layer.h"

#include "../../graphics/uimodel.h"
#include "../../io/input.h"
#include "../../utilities/utilities.h"
#include "../../logging/logging.h"

namespace feng::ui {

	ui_layer::ui_layer(uint8_t render_order)
		: render_order(render_order) { }

	std::shared_ptr<uiinstance> ui_layer::add_instance(uimodel* model, glm::vec2 pos_in_ndc, glm::vec2 size) {
		std::shared_ptr<uiinstance> inst = model->add_instance(this, pos_in_ndc, size);
		_instances.push_back(inst);
		return inst;
	}

	void ui_layer::start() {
		for (const auto& inst : _instances)
			inst->start();
	}

	void ui_layer::update() {
		for (const auto& inst : _instances)
			inst->update();
	}

	std::tuple<uii_sprt, uii_sprt, uii_sprt, uii_sprt> ui_layer::process_input() {
		uii_sprt click_instance, mouse_up_instance, mouse_down_instance, mouse_in_instance;
		for (const auto& instance : _instances) {
			if (instance->is_active) {
				if (input::get_mouse_key_up(GLFW_MOUSE_BUTTON_LEFT)) {
					if (instance->process_click &&
						instance->bounds.intersects_with(input::get_click_start_pos()) &&
						instance->bounds.intersects_with(input::get_mouse_pos_ndc())) {
						if (!click_instance.get())
							click_instance = instance;
						else if (click_instance->render_order > instance->render_order)
							click_instance = instance;
					}
					if (instance->process_mouse_up && 
						instance->bounds.intersects_with(input::get_mouse_pos_ndc())) {
						if (!mouse_up_instance.get())
							mouse_up_instance = instance;
						else if (mouse_up_instance->render_order > instance->render_order)
							mouse_up_instance = instance;
					}
				}
				if (input::get_mouse_key_down(GLFW_MOUSE_BUTTON_LEFT)) {
					if (instance->process_mouse_down &&
						instance->bounds.intersects_with(input::get_mouse_pos_ndc())) {
						if (!mouse_down_instance.get())
							mouse_down_instance = instance;
						else if (mouse_down_instance->render_order > instance->render_order)
							mouse_down_instance = instance;
					}
				}
				if (instance->process_mouse_in &&
					instance->bounds.intersects_with(input::get_mouse_pos_ndc())) {
					if (!mouse_in_instance.get())
						mouse_in_instance = instance;
					else if (mouse_in_instance->render_order > instance->render_order)
						mouse_in_instance = instance;
				}
			}
		}
		return { click_instance, mouse_up_instance, mouse_down_instance, mouse_in_instance };
	}

}