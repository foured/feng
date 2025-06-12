#include "lightbaker.h"

#include <array>

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
			std::shared_ptr<instance> receiver_instance, caster_instance;
			std::shared_ptr<model_instance> receiver_model_instance, caster_model_instance;
			dir_light* light = &scene->dir_light;
			glm::mat4 model = scene->model_matrix;
			assets_manager* am = assets_manager::get_instance();

			//scene->calculate_projection_matrix();

			data::wfile file(path);
			file.write_raw(scene->get_uuid());
			file.write_raw(receivers.size());

			for (const auto& receiver_pair : receivers) {
				receiver_instance = receiver_pair.first;
				receiver_model_instance = receiver_pair.second;

				int32_t shadow_texture_res = calculate_shadow_texture_res(receiver_pair);

				std::array<texture, 2> shadow_textures = std::array<texture, 2>();
				std::array<framebuffer, 2> shadow_framebuffers = {
					framebuffer(shadow_texture_res, shadow_texture_res),
					framebuffer(shadow_texture_res, shadow_texture_res)
				};
				
				for (size_t i = 0; i < 2; i++) {
					shadow_textures[i].generate();
					shadow_textures[i].bind();
					shadow_textures[i].allocate(shadow_texture_res, shadow_texture_res, GL_R32F, GL_RED, GL_FLOAT);
					shadow_textures[i].set_params(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
				
					shadow_framebuffers[i].bind();
					shadow_framebuffers[i].attach_texture2d(shadow_textures[i], GL_COLOR_ATTACHMENT0);
					shadow_framebuffers[i].unbind();
				}

				//texture shadow_texture;
				//shadow_texture.generate();
				//shadow_texture.bind();
				//shadow_texture.allocate(shadow_texture_res, shadow_texture_res, GL_RED, GL_RED, GL_UNSIGNED_BYTE);
				//shadow_texture.set_params(GL_NEAREST, GL_NEAREST);
				//
				//framebuffer shadow_framebuffer(shadow_texture_res, shadow_texture_res);
				//shadow_framebuffer.bind();
				//shadow_framebuffer.attach_texture2d(shadow_texture, GL_COLOR_ATTACHMENT0);
				//shadow_framebuffer.unbind();

				bool t = false;
				for (const auto& caster_pair : casters) {
					caster_instance = caster_pair.first;
					caster_model_instance = caster_pair.second;

					if (caster_instance->get_uuid() != receiver_instance->get_uuid()) {
						light->lightspace_matrix = light->generate_custom_relative_lightspace_matrix(
							caster_model_instance->calculate_bounds(),
							receiver_model_instance->calculate_bounds(),
							model
						);
						light->full_render_preparations(am->shaders.dirlight_depth_shader, model);
						caster_model_instance->render_alone(am->shaders.dirlight_depth_shader);
						light->render_cleanup();

						shadow_framebuffers[t].set_viewport();
						shadow_framebuffers[t].bind();

						glDisable(GL_DEPTH_TEST);
						glClear(GL_COLOR_BUFFER_BIT);

						glCullFace(GL_FRONT);

						am->shaders.light_bake.activate();
						am->shaders.light_bake.set_mat4("model", model);
						am->shaders.light_bake.set_mat4("lightSpaceMatrix", light->lightspace_matrix);
						am->shaders.light_bake.set_int("shadowMap", 31);
						light->bind_shadowmap(31);
						am->shaders.light_bake.set_int("inputTexture", 30);
						shadow_textures[!t].bind_to_slot(30);

						receiver_model_instance->render_alone(am->shaders.light_bake);

						shadow_framebuffers[t].unbind();

						t = !t;
					}
				}
				file.write_raw(receiver_instance->get_uuid());
				file.write_serializable(&shadow_textures[t]);

				for (size_t i = 0; i < 2; i++) {
					shadow_textures[i].delete_buffer();
					shadow_framebuffers[i].delete_buffer();
				}
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
			shadow_texture_resolution = 4096;
		return shadow_texture_resolution;
	}

}