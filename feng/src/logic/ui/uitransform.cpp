#include "uitransform.h"

#include "../../logging/logging.h"
#include "../../utilities/utilities.h"
#include "uibounds.h"
#include "uiinstance.h"
#include "../../graphics/window.h"

namespace feng::ui {

	uitransform::uitransform(uiinstance* instance) 
		: instance(instance) { }

	void uitransform::set_anchor(anchor anchor) {
		glm::vec2 sp = _position;
		_position = {0, 0};
		_anchor = anchor;
		_position += _get_anchor_offset();
		instance->set_children_pos(_position - sp);
	}

	void uitransform::set_pos_pix(glm::vec2 pos_in_pix) {
		set_pos_ndc(utilities::pixel_to_ndc(pos_in_pix));
	}

	void uitransform::set_pos_ndc(glm::vec2 pos_in_ndc) {
		glm::vec2 sp = _position;
		_position = pos_in_ndc;
		_position += _get_anchor_offset();
		instance->set_children_pos(_position - sp);
	}

	glm::vec2 uitransform::get_pos_ndc() {
		return _position;
	}

	glm::vec2 uitransform::get_pos_pix() {
		return utilities::ndc_to_pixel(_position);
	}

	void uitransform::set_size_pix(glm::vec2 size_in_pix) {
		uibounds_rect rect = instance->bounds.rect();
		float w = rect.b_max.x - rect.b_min.x;
		float h = rect.b_max.y - rect.b_min.y;
		glm::vec2 ndc = utilities::pixel_to_ndc(size_in_pix) * 2.0f;
		_size = glm::vec2(ndc.x / w, ndc.y / h);
	}

	void uitransform::set_size(glm::vec2 size) {
		_size = size;
	}

	glm::vec2 uitransform::get_size_ndc() {
		return _size;
	}

	glm::vec2 uitransform::_get_anchor_offset() {
		float v = 1.0f;
		glm::vec2 ret(0.0f);
		switch (_anchor)
		{
		case feng::ui::TOP:
			ret = { 0, v };
			break;

		case feng::ui::CENTER:
			break;

		case feng::ui::BOTTOM:
			ret = { 0, -v };
			break;

		case feng::ui::LEFT:
			ret = { -v, 0 };
			break;

		case feng::ui::RIGHT:
			ret = { v, 0 };
			break;

		case feng::ui::TOP_LEFT:
			ret = { v, -v };
			break;

		case feng::ui::TOP_RIGHT:
			ret = { v, v };
			break;

		case feng::ui::BOTTOM_LEFT:
			ret = { -v, -v };
			break;

		case feng::ui::BOTTOM_RIGHT:
			ret = { v, -v };
			break;

		default:
			LOG_WARNING("Unknown anchor type detected. Implement it in uitransform.cpp");
			break;
		}

		return ret;
	}

}