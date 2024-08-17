#pragma once

#include <glm/glm.hpp>

namespace feng::ui {

	class uiinstance;

	enum anchor {
		TOP,
		CENTER,
		BOTTOM,
		LEFT,
		RIGHT,
		TOP_LEFT,
		TOP_RIGHT,
		BOTTOM_LEFT,
		BOTTOM_RIGHT
	};

	class uitransform {
	public:
		uitransform(uiinstance* instance);

		uiinstance* instance;

		void set_anchor(anchor anchor);

		void set_pos_pix(glm::vec2 pos_in_pix);
		void set_pos_ndc(glm::vec2 pos_in_ndc);
		glm::vec2 get_pos_ndc();
		glm::vec2 get_pos_pix();

		void set_size_pix(glm::vec2 size_in_pix);
		void set_size(glm::vec2 size);
		glm::vec2 get_size_ndc();

	private:
		glm::vec2 _position = glm::vec2(0);
		glm::vec2 _size = glm::vec2(1);

		anchor _anchor = anchor::CENTER;

		glm::vec2 _get_anchor_offset();

	};

}