#include "ui.h"

namespace feng::ui {

	ui::ui(shader& shader)
		: _shader(shader), _ortho_proj(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, (float)UCHAR_MAX)) {	}

	std::shared_ptr<uimodel> ui::create_model(std::vector<mesh2d> meshes) {
		std::shared_ptr<uimodel> uptr_model = std::make_shared<uimodel>(meshes);
		_models.emplace_back(uptr_model);
		return uptr_model;
	}

	std::shared_ptr<ui_layer> ui::create_layer(uint8_t render_order) {
		std::shared_ptr<ui_layer> layer_uptr = std::make_shared<ui_layer>(render_order);
		_layers.emplace_back(layer_uptr);
		return layer_uptr;
	}

	std::shared_ptr<uiinstance> ui::add_instance(
		ui_layer* layer,
		uimodel* model,
		glm::vec2 pos_in_ndc,
		glm::vec2 size) {
		return layer->add_instance(model, pos_in_ndc, size);
	}

	std::shared_ptr<uiinstance> ui::add_instance(
		std::shared_ptr<ui_layer> layer,
		std::shared_ptr<uimodel> model,
		glm::vec2 pos_in_ndc,
		glm::vec2 size) {
		return layer->add_instance(model.get(), pos_in_ndc, size);
	}

	void ui::start() {
		for (const auto& layer : _layers)
			layer->start();
	}

	void ui::update() {
		uii_sprt click_instance, mouse_up_instance, mouse_down_instance, mouse_in_instance;
		for (const auto& layer : _layers) {
			if (layer->is_active) {
				if (layer->support_input) {
					auto[cci, cmui, cmdi, cmii] = layer->process_input();
					if (cci.get()) {
						if (!click_instance.get())
							click_instance = cci;
						else if (click_instance->calculate_z() < cci->calculate_z())
							click_instance = cci;
					}
					if (cmui.get()) {
						if (!mouse_up_instance.get())
							mouse_up_instance = cmui;
						else if (mouse_up_instance->calculate_z() < cmui->calculate_z())
							mouse_up_instance = cmui;
					}
					if (cmdi.get()) {
						if (!mouse_down_instance.get())
							mouse_down_instance = cmdi;
						else if (mouse_down_instance->calculate_z() < cmdi->calculate_z())
							mouse_down_instance = cmdi;
					}
					if (cmii.get()) {
						if (!mouse_in_instance.get())
							mouse_in_instance = cmii;
						else if(mouse_in_instance->calculate_z() < cmii->calculate_z())
							mouse_in_instance = cmii;
					}
				}
			}
		}
		if (click_instance.get())
			click_instance->on_click.invoke();
		if (mouse_up_instance.get())
			mouse_up_instance->on_mouse_up.invoke();
		if (mouse_down_instance.get())
			mouse_down_instance->on_mouse_down.invoke();
		if (mouse_in_instance.get())
			mouse_in_instance->on_mouse_in.invoke();

		for (const auto& layer : _layers)
			layer->update();
	}

	void ui::render() {
		glClear(GL_DEPTH_BUFFER_BIT);
		_shader.activate();
		_shader.set_mat4("projection", _ortho_proj);
		for (const auto& model : _models) {
			model->render(_shader);
		}
	}

	glm::mat4 ui::get_projection_matrix() const {
		return _ortho_proj;
	}

}