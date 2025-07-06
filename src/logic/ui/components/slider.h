#pragma once

#include "../uicomponent.h"
#include "../../event.hpp"

namespace feng::ui {

	class slider : public uicomponent {
	public:
		slider(uiinstance* instance, uiinstance* pin);

		void start();
		void update();

		float get_value();
		void set_value(float value);
		float get_real_value();

	private:
		uiinstance* _pin;
		bool _is_lmb_down = false;

		feng::event<>::subscription _pin_sub;

		float calculate_w();

	};

}