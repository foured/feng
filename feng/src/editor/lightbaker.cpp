#include "lightbaker.h"

#include "../logic/data_management/assets_manager.h"
#include "../logic/data_management/files.h"
#include "../graphics/window.h"

#define LIGHT_BAKER_SHADOWMAP_SIZE 2048

namespace feng::editor {


	void lightbaker::bake(scene* scene, const std::filesystem::path& path) {
		std::vector<im_pair> casters;
		std::vector<im_pair> receivers;
		for (const auto& instance : scene->_instances) {
			std::shared_ptr<model_instance> m_instance = instance->try_get_component<model_instance>();
			if (m_instance && instance.get()->flags.get(INST_FLAG_STATIC)) {
				if (instance.get()->flags.get(INST_FLAG_RCV_SHADOWS))
					receivers.emplace_back(instance, m_instance);
				if (instance.get()->flags.get(INST_FLAG_CAST_SHADOWS))
					casters.emplace_back(instance, m_instance);
			}
		}
		
		if (!casters.empty() && !receivers.empty()) {
			timer lightbake_time("Light bake timer");
			LOG_INFO("Starting baking light");

			data::wfile file(path);
			file.write_raw(scene->get_uuid());
			file.write_raw(receivers.size());

			dir_light* light = nullptr;
			assets_manager* manager = assets_manager::get_instance();
			std::shared_ptr<model_instance> caster_m_instance;
			aabb model_bounds;
			scene->calculate_projection_matrix();

			std::vector<instance*> pointers;
			std::vector<dir_light*> lights;

			for (const auto& caster_pair : casters) {
				pointers.push_back(caster_pair.first.get());
				lights.push_back(new dir_light(
					scene->dir_light.direction,
					scene->dir_light.ambient,
					scene->dir_light.diffuse,
					scene->dir_light.specular,
					LIGHT_BAKER_SHADOWMAP_SIZE));

				light = lights.back();
				light->generate_buffers();

				caster_m_instance = caster_pair.second;
				model_bounds = caster_m_instance.get()->get_model()->bounds;

				transform* i_transform = &caster_pair.first.get()->transform;
				glm::vec3 min = model_bounds.min * i_transform->get_size() + i_transform->get_position();
				glm::vec3 max = model_bounds.max * i_transform->get_size() + i_transform->get_position();

				light->lightspace_matrix = light->generate_custom_lightspace_matrix(min, max, scene->model_matrix);
				//light->generate_lightspace_matrix();
				light->full_render_preparations(manager->shaders.dirlight_depth_shader, scene->model_matrix);
				caster_m_instance->render_alone(manager->shaders.dirlight_depth_shader);
				light->render_cleanup();

				//std::string shadowmap_path = std::to_string(pointers.back()->get_uuid()) + ".png";
				//light->_shadowmap.save_to_png(shadowmap_path);
			}

			for (const auto& pair : receivers) {
				std::shared_ptr<model_instance> receiver_m_instance = pair.second;
				std::shared_ptr<instance> receiver_instance = pair.first;
				int32_t shadow_texture_res = calculate_shadow_texture_res(pair);
				texture shadow_texture;
				shadow_texture.generate();
				shadow_texture.bind();
				std::vector<uint8_t> pixels(shadow_texture_res * shadow_texture_res, 0);
				shadow_texture.allocate(shadow_texture_res, shadow_texture_res, GL_RED, GL_RED, GL_UNSIGNED_BYTE, pixels.data());
				shadow_texture.set_params(GL_NEAREST, GL_NEAREST);

				framebuffer shadow_framebuffer(shadow_texture_res, shadow_texture_res);
				shadow_framebuffer.bind();
				shadow_framebuffer.attach_texture2d(shadow_texture, GL_COLOR_ATTACHMENT0);
				shadow_framebuffer.unbind();

				for (size_t i = 0; i < lights.size(); i++) {
					if (receiver_instance.get() != pointers[i]) {
						light = lights[i];

						shadow_framebuffer.set_viewport();
						shadow_framebuffer.bind();

						glDisable(GL_DEPTH_TEST);
						//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
						glClear(GL_COLOR_BUFFER_BIT);

						manager->shaders.light_bake.activate();
						manager->shaders.light_bake.set_mat4("model", scene->model_matrix);
						manager->shaders.light_bake.set_mat4("view", scene->main_camera.get_view_matrix());
						manager->shaders.light_bake.set_mat4("lightSpaceMatrix", light->lightspace_matrix);
						manager->shaders.light_bake.set_mat4("projection", scene->get_projection_matrix());
						manager->shaders.light_bake.set_int("shadowMap", 0);
						light->bind_shadowmap(0);
						manager->shaders.light_bake.set_int("inputTexture", 1);
						texture::activate_slot(1);
						shadow_texture.bind();
						receiver_m_instance->render_alone(manager->shaders.light_bake);

						shadow_framebuffer.unbind();
					}
				}

				file.write_raw(receiver_instance->get_uuid());
				file.write_serializable(&shadow_texture);

				shadow_texture.del();
				shadow_framebuffer.delete_buffer();
			}

			for (dir_light* dl : lights) {
				dl->delete_buffers();
				delete dl;
			}
		}
	}

	int32_t lightbaker::calculate_shadow_texture_res(const im_pair& pair){
		aabb model_bounds = pair.second.get()->get_model()->bounds;
		transform* i_transform = &pair.first.get()->transform;
		glm::vec3 min = model_bounds.min * i_transform->get_size();
		glm::vec3 max = model_bounds.max * i_transform->get_size();
		glm::vec3 size = glm::abs(max - min);
		float volume = size.x * size.y * size.z;
		uint32_t shadow_texture_resolution;
		if (volume <= 1)
			shadow_texture_resolution = 64;
		else if (volume <= 2)
			shadow_texture_resolution = 128;
		else if (volume <= 10)
			shadow_texture_resolution = 256;
		else if (volume <= 500)
			shadow_texture_resolution = 512;
		else
			shadow_texture_resolution = 1024;
		return shadow_texture_resolution;
	}

}