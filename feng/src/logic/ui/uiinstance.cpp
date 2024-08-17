#include "uiinstance.h"

#include "../../graphics/uimodel.h"
#include "ui_layer.h"

namespace feng::ui {

	uiinstance::uiinstance(feng::uimodel* model, ui_layer* layer)
		: _model(model), _layer(layer), uitransform(this), bounds(model->rect(), &uitransform) { }

	float uiinstance::calculate_z() {
		float ret = _layer->render_order;
		ret += 1.0f / (UCHAR_MAX + 1) * render_order;
		return -ret;
	}

	void uiinstance::set_all_processes(float value) {
		process_click = value;
		process_mouse_up = value;
		process_mouse_down = value;
		process_mouse_in = value;
	}

	bool uiinstance::is_layer_active() const {
		return _layer->is_active;
	}

	void uiinstance::start() {
		for (auto& component : _components)
			component->start();
	}

	void uiinstance::update() {
		for (auto& component : _components)
			component->update();
	}

	void uiinstance::add_child(uiinstance* child) {
		if (std::find(_children.begin(), _children.end(), child) == _children.end())
			_children.push_back(child);
	}

	void uiinstance::set_children_pos(glm::vec2 offset) {
		cleanup_children();
		for (auto child : _children) 
			child->uitransform.set_pos_ndc(child->uitransform.get_pos_ndc() + offset);
	}

	void uiinstance::cleanup_children() {
		_children.erase(
			std::remove_if(_children.begin(), _children.end(),
				[](uiinstance* ptr) { return ptr == nullptr; }),
			_children.end());
	}

}