#include "lights.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <vector>

#include "../../utilities/utilities.h"

namespace feng {

	dir_light::dir_light(uint32_t shadowmap_size) 
		: _shadowmap_size(shadowmap_size) { }

	dir_light::dir_light(const glm::vec3& dir, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular,
		uint32_t shadowmap_size)
		: _shadowmap_size(shadowmap_size), direction(glm::normalize(dir)), ambient(ambient), diffuse(diffuse), specular(specular) {	}

	void dir_light::generate_lightspace_matrix() {
		float border = 1;
		float dlm_near_plane = 0.0f, dlm_far_plane = 20.0f;
		glm::mat4 light_projection = glm::ortho(-border, border, -border, border, dlm_near_plane, dlm_far_plane);
		glm::vec3 center(0.0f, 0.0f, 0.0f);
		glm::vec3 pos = center -3.0f * direction;
		glm::mat4 light_view = glm::lookAt(
			pos,
			center,
			glm::vec3(0.0f, 1.0f, 0.0f));

		lightspace_matrix = light_projection * light_view;
	}

	glm::mat4 dir_light::generate_custom_lightspace_matrix(const glm::vec3& min, const glm::vec3& max, const glm::mat4& model) {
		glm::vec3 center = (min + max) * 0.5f;
		float radius = glm::length(max - min) * 0.5f;

		glm::vec3 light_pos = center - direction * radius * 2.0f;
		glm::mat4 light_view = glm::lookAt(light_pos, center, glm::vec3(0, 1, 0));


		auto [min_ls, max_ls] = utilities::calculate_min_max_light_space(min, max, light_view, model);

		float l = min_ls.x, r = max_ls.x;
		float b = min_ls.y, t = max_ls.y;
		float n = -max_ls.z, f = -min_ls.z;

		glm::mat4 light_proj = glm::ortho(l, r, b, t, n, f);
		return light_proj * light_view;
	}


	glm::mat4 dir_light::generate_custom_lightspace_matrix(const aabb& bounds, const glm::mat4& model) {
		return generate_custom_lightspace_matrix(bounds.min, bounds.max, model);
	}

	glm::mat4 dir_light::generate_custom_relative_lightspace_matrix(const glm::vec3& cmin, const glm::vec3& cmax,
		const glm::vec3& rmin, const glm::vec3& rmax, const glm::mat4& model) {
		glm::vec3 center = (cmin + cmax) * 0.5f;
		float radius = glm::length(cmax - cmin) * 0.5f;

		glm::vec3 light_pos = center - direction * radius * 2.0f;
		glm::mat4 light_view = glm::lookAt(light_pos, center, glm::vec3(0, 1, 0));

		auto [cmin_ls, cmax_ls] = utilities::calculate_min_max_light_space(cmin, cmax, light_view, model);
		auto [rmin_ls, rmax_ls] = utilities::calculate_min_max_light_space(rmin, rmax, light_view, model);

		float l = cmin_ls.x, r = cmax_ls.x;
		float b = cmin_ls.y, t = cmax_ls.y;

		float cn = -cmax_ls.z, cf = -cmin_ls.z;
		float rn = -rmax_ls.z, rf = -rmin_ls.z;

		float n = std::min(cn, rn), f = std::max(cf, rf);

		glm::mat4 light_proj = glm::ortho(l, r, b, t, n, f);
		return light_proj * light_view;
	}

	glm::mat4 dir_light::generate_custom_relative_lightspace_matrix(const aabb& caster, const aabb& receiver, const glm::mat4& model) {
		return generate_custom_relative_lightspace_matrix(caster.min, caster.max, receiver.min, receiver.max, model);
	}

	void dir_light::generate_buffers() {
		_depthmap_framebuffer = framebuffer(_shadowmap_size, _shadowmap_size);
		_depthmap_framebuffer.bind();
		_shadowmap = _depthmap_framebuffer.allocate_and_attach_texture(
			GL_DEPTH_ATTACHMENT, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT);
		const float border_color[] = { 1.0, 1.0, 1.0, 1.0 };
		_shadowmap.set_param_fv(GL_TEXTURE_BORDER_COLOR, border_color);
		_depthmap_framebuffer.set_draw_buffer(GL_NONE);
		_depthmap_framebuffer.set_read_buffer(GL_NONE);
		framebuffer::check_status();
		_depthmap_framebuffer.unbind();
	}

	void dir_light::render_preparations() {
		_depthmap_framebuffer.set_viewport();
		_depthmap_framebuffer.bind();
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		//glDisable(GL_CULL_FACE);
		glCullFace(GL_FRONT); // Use for PCSS
	}

	void dir_light::full_render_preparations(shader& shader, glm::mat4 model) {
		render_preparations();
		shader.activate();
		shader.set_mat4("lightSpaceMatrix", lightspace_matrix);
		shader.set_mat4("model", model);
	}

	void dir_light::render_cleanup() {
		//glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK); // Use for PCSS
		_depthmap_framebuffer.unbind();
	}

	void dir_light::bind_shadowmap(uint32_t slot) {
		texture::activate_slot(slot);
		_shadowmap.bind();
	}

	texture dir_light::get_texture() const {
		return _shadowmap;
	}

	void dir_light::delete_buffers() {
		_shadowmap.delete_buffer();
		_depthmap_framebuffer.delete_buffer();
	}

	point_light::point_light(uint32_t shadowmap_size)
		: _shadowmap_size(shadowmap_size) {}


	point_light::point_light(const glm::vec3& pos, float cons, float lin, float quad, const glm::vec3& amb,
		const glm::vec3& diff, const glm::vec3& spec, uint32_t shadowmap_size) : position(pos),
		constant(cons), linear(lin), quadratic(quad), ambient(amb), diffuse(diff), specular(spec),
		_shadowmap_size(shadowmap_size) {}

	void point_light::generate_lightspace_matrices() {
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, far_plane);

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

	void point_light::generate_buffers() {
		_shadowmap = cubemap(_shadowmap_size, _shadowmap_size, GL_DEPTH_COMPONENT);
		_framebuffer = framebuffer(_shadowmap_size, _shadowmap_size);
		_framebuffer.bind();
		_framebuffer.attach_texture(_shadowmap.id(), GL_DEPTH_ATTACHMENT);
		_framebuffer.set_draw_buffer(GL_NONE);
		_framebuffer.set_read_buffer(GL_NONE);
		framebuffer::check_status();
		_framebuffer.unbind();
	}

	void point_light::render_preparations(shader& shader) {
		_framebuffer.set_viewport();
		_framebuffer.bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		shader.activate();
		for (uint32_t i = 0; i < 6; i++)
			shader.set_mat4("shadowMatrices[" + std::to_string(i) + "]", lightspace_matrices[i]);
		shader.set_float("far_plane", far_plane);
		shader.set_3float("lightPos", position);
	}

	void point_light::render_cleanup() {
		_framebuffer.unbind();
	}

	void point_light::bind_shadowmap(uint32_t slot) {
		texture::activate_slot(slot);
		_shadowmap.bind();
	}

}