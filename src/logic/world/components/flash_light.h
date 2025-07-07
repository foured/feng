#pragma once

#include "../component.h"
#include <glm/glm.hpp>

namespace feng {

	class flash_light : public component {
	public:
		struct static_data {
			float cut_off;
			float outer_cut_off;

			float constant;
			float linear;
			float quadratic;

			glm::vec3 ambient;
			glm::vec3 diffuse;
			glm::vec3 specular;
		};

		flash_light(instance* instance, scene* scene, 
			float cut_off = glm::cos(glm::radians(12.5f)), float outer_cut_off = glm::cos(glm::radians(17.5f)),
			float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f,
			glm::vec3 ambient = glm::vec3(0.1f), glm::vec3 diffuse = glm::vec3(0.8f), glm::vec3 specular = glm::vec3(0.5f));

		flash_light(instance* instance, scene* scene, static_data data);
		
		void start() override;
		void late_start() override;
		void update() override;
		std::shared_ptr<component> copy(instance* new_instance) override;

		void serialize(data::wfile* file) override;
		void deserialize(data::rfile* file, scene* scene) override;

	private:
		flash_light(instance* instance);

		scene* _scene;
		size_t _idx;

		static_data _static;

		void update_dynamic_data();
		void update_static_data();

	};

}