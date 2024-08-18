#include "slider.h"

#include <glm/glm.hpp>

#include "../uiinstance.h"
#include "../../../logging/logging.h"
#include "../../../io/input.h"

namespace feng::ui {

	slider::slider(uiinstance* instance, uiinstance* pin)
		: uicomponent(instance), _pin(pin) {
		_instance->add_child(pin);
		_pin->uitransform.set_pos_ndc(_instance->uitransform.get_pos_ndc());
		_pin->set_all_processes(true);
		if (_instance->render_order == 0)
			LOG_WARNING("render_order of slider instance cant be 0");
		_pin->render_order = _instance->render_order - 1;
		_pin->on_mouse_down.subscribe([this]() 
			{ 
				if(is_active)
					_is_lmb_down = true; 
			});
	}

	void slider::start() {

	}

	void slider::update() {
		if (is_active && input::get_mouse_key_up(GLFW_MOUSE_BUTTON_LEFT)) _is_lmb_down = false;
		if (is_active && input::get_mouse_key(GLFW_MOUSE_BUTTON_LEFT)) {
			if (_is_lmb_down) {
				float v = get_real_value();

				if (v < 0) set_value(0);
				else if (v > 1) set_value(1);

				_pin->uitransform.set_pos_pix(_pin->uitransform.get_pos_pix()
					+ glm::vec2(input::get_mouse_delta_pos().x * 2.0f, 0));
			}
		}
	}

	float slider::calculate_w() {
		float w = _instance->bounds.real_rect().b_max.x - _instance->bounds.real_rect().b_min.x;
		float pw = _pin->bounds.real_rect().b_max.x - _pin->bounds.real_rect().b_min.x;
		w -= pw;
		return w;
	}

	float slider::get_value() {
		float value = get_real_value();
		if (value < 0.0f) value = 0.0f;
		if (value > 1.0f) value = 1.0f;
		return value;
	}

	float slider::get_real_value() {
		float w = calculate_w();
		float dp = _instance->uitransform.get_pos_ndc().x - _pin->uitransform.get_pos_ndc().x + (w / 2);
		float ret = dp / w;
		return 1 - ret;
	}

	void slider::set_value(float value) {
		if (value < 0.0f) value = 0.0f;
		if (value > 1.0f) value = 1.0f;

		float w = calculate_w();
		float pin_position_x = _instance->uitransform.get_pos_ndc().x - (w / 2) + (w * value);
		_pin->uitransform.set_pos_ndc({ pin_position_x, _pin->uitransform.get_pos_ndc().y });
	}

}