#include "lights.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

namespace feng {

	DirLight::DirLight(uint32_t shadowmap_size) 
		: _shadowmap_size(shadowmap_size) { }

	DirLight::DirLight(const glm::vec3& dir, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular,
		uint32_t shadowmap_size)
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

	PointLight::PointLight(uint32_t shadowmap_size)
		: _shadowmap_size(shadowmap_size) {}


	PointLight::PointLight(const glm::vec3& pos, float cons, float lin, float quad, const glm::vec3& amb,
		const glm::vec3& diff, const glm::vec3& spec, uint32_t shadowmap_size) : position(pos),
		constant(cons), linear(lin), quadratic(quad), ambient(amb), diffuse(diff), specular(spec),
		_shadowmap_size(shadowmap_size) {}

	void PointLight::generate_lightspace_matrices() {
		float near = 0.1f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, near, far_plane);

		lightspace_matrices[0] = shadowProj *
			glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
		lightspace_matrices[1] = shadowProj *
			glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
		lightspace_matrices[2] = shadowProj *
			glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
		lightspace_matrices[3] = shadowProj *
			glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
		lightspace_matrices[4] = shadowProj *
			glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
		lightspace_matrices[5] = shadowProj *
			glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));
	}

	void PointLight::generate_buffers() {
		_shadowmap = cubemap(_shadowmap_size, _shadowmap_size, GL_DEPTH_COMPONENT);
		_framebuffer = framebuffer(_shadowmap_size, _shadowmap_size);
		_framebuffer.bind();
		_framebuffer.attach_texture(_shadowmap.id(), GL_DEPTH_ATTACHMENT);
		_framebuffer.set_draw_buffer(GL_NONE);
		_framebuffer.set_read_buffer(GL_NONE);
		framebuffer::check_status();
		_framebuffer.unbind();
	}

	void PointLight::render_preparations(shader& shader) {
		_framebuffer.set_viewport();
		_framebuffer.bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		shader.activate();
		for (uint32_t i = 0; i < 6; i++)
			shader.set_mat4("shadowMatrices[" + std::to_string(i) + "]", lightspace_matrices[i]);
		shader.set_float("far_plane", far_plane);
		shader.set_3float("lightPos", position);
	}

	void PointLight::render_cleanup() {
		_framebuffer.unbind();
	}

	void PointLight::bind_shadowmap(uint32_t slot) {
		texture::activate_slot(slot);
		_shadowmap.bind();
	}

}