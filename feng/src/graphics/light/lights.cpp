#include "lights.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

namespace feng {

	DirLight::DirLight(uint32_t shadowmap_size) 
		: _shadowmap_size(shadowmap_size) { }

	DirLight::DirLight(glm::vec3 dir, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, uint32_t shadowmap_size)
		: _shadowmap_size(shadowmap_size), direction(dir), ambient(ambient), diffuse(diffuse), specular(specular) {	}

	glm::mat4 DirLight::generate_lightspace_matrix() {
		float border = 5;
		float dlm_near_plane = 0.0f, dlm_far_plane = 30.5f;
		glm::mat4 light_projection = glm::ortho(-border, border, -border, border, dlm_near_plane, dlm_far_plane);
		glm::vec3 pos = -3.0f * direction;
		glm::mat4 light_view = glm::lookAt(
			pos,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

		return light_projection * light_view;
	}

	void DirLight::generate_buffers() {
		_depthmap_framebuffer = framebuffer(_shadowmap_size, _shadowmap_size);
		_depthmap_framebuffer.bind();
		_shadowmap = _depthmap_framebuffer.allocate_and_attach_texture(
			GL_DEPTH_ATTACHMENT, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER, GL_DEPTH_COMPONENT);
		const float border_color[] = { 1.0, 1.0, 1.0, 1.0 };
		_shadowmap.set_param_fv(GL_TEXTURE_BORDER_COLOR, border_color);
		_depthmap_framebuffer.set_draw_buffer(GL_NONE);
		_depthmap_framebuffer.set_read_buffer(GL_NONE);
		framebuffer::check_status();
		_depthmap_framebuffer.unbind();
	}

	void DirLight::render_preparations() {
		_depthmap_framebuffer.set_viewport();
		_depthmap_framebuffer.bind();
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
	}

	void DirLight::render_cleanup() {
		glCullFace(GL_BACK);
		_depthmap_framebuffer.unbind();
	}

	void DirLight::bind_shadowmap(uint32_t slot) {
		texture::activate_slot(slot);
		_shadowmap.bind();
	}

}