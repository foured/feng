#pragma once

#include <vector>

#include "uitransform.h"
#include "uibounds.h"

#include "../event.hpp"
#include "uicomponent.h"

namespace feng {
	class uimodel;

	namespace ui {
		class ui_layer;
	}
}

namespace feng::ui{
	using uii_sprt = std::shared_ptr<uiinstance>;

	class uiinstance {
	public:
		uiinstance(uimodel* model, ui_layer* layer);

		uint8_t render_order = 1;
		uitransform uitransform;
		uibounds bounds;
		bool is_active = true;
		bool process_click = false;
		bool process_mouse_up = false;
		bool process_mouse_down = false;
		bool process_mouse_in = false;
		event<> on_click;
		event<> on_mouse_up;
		event<> on_mouse_down;
		event<> on_mouse_in;

		float calculate_z();
		bool is_layer_active() const;
		void set_all_processes(float value);
		void start();
		void update();
		void add_child(uiinstance* child);
		void set_children_pos(glm::vec2 offset);

		template<typename T, typename ...Args>
		typename std::enable_if<std::is_base_of<uicomponent, T>::value, T&>::type
		add_component(Args... args) {
			auto component = std::make_unique<T>(this, std::forward<Args>(args)...);
			T& ref = *component;
			_components.emplace_back(std::move(component));
			return ref;
		}

	private:
		uimodel* _model;
		ui_layer* _layer;
		std::vector<std::unique_ptr<uicomponent>> _components;
		std::vector<uiinstance*> _children;

		void cleanup_children();

	};

}