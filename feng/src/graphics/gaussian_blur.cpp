#ifdef IMPLEMENT_WORK_IN_PROGRESS_CODE

#include "gaussian_blur.h"

#include "../logic/data_management/assets_manager.h"

namespace feng {

	gaussian_blur::gaussian_blur(helpers::fullscreen_quad* fs_quad, texture* input_texture)
		: _shader(&assets_manager::get_instance()->shaders.gaussian_blur_shader), _fs_quad(fs_quad), _input_texture(input_texture) { 
		int32_t w = _input_texture->width();
		int32_t h = _input_texture->height();
		_framebuffers = {
			framebuffer(w, h),
			framebuffer(w, h)
		};

		_input_texture->bind();
		int32_t min_f = _input_texture->get_param_iv(GL_TEXTURE_MIN_FILTER);
		int32_t mag_f = _input_texture->get_param_iv(GL_TEXTURE_MAG_FILTER);
		int32_t wrap_s = _input_texture->get_param_iv(GL_TEXTURE_WRAP_S);
		int32_t wrap_t = _input_texture->get_param_iv(GL_TEXTURE_WRAP_T);

		_textures = std::array<texture, 2>();
		for (int32_t i = 0; i < 2; i++) {
			_framebuffers[i].bind();
			_textures[i].generate();
			_textures[i].bind();
			_textures[i].allocate(w, h, _input_texture->internal_format(), _input_texture->format(), _input_texture->type());
			_textures[i].set_params(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

			_framebuffers[i].attach_texture2d(_textures[i], GL_COLOR_ATTACHMENT0);
			framebuffer::check_status();
		}
		_framebuffers[0].unbind();
	}

	gaussian_blur::~gaussian_blur() {

	}

	void gaussian_blur::draw(uint32_t iterations) {
		shader* shader = &assets_manager::get_instance()->shaders.gaussian_blur_shader;

		bool horizontal = true, first_iteration = true;
		_framebuffers[0].set_viewport();
		glDisable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT);
		shader->activate();
		shader->set_int("inputTexture", 0);
		texture::activate_slot(0);
		for (uint32_t i = 0; i < iterations; i++) {
			_framebuffers[horizontal].bind();
			shader->set_bool("horizontal", horizontal);
			if (first_iteration)
				_input_texture->bind();
			else
				_textures[!horizontal].bind();

			_fs_quad->draw_render_data();

			horizontal = !horizontal;
			if (first_iteration)
				first_iteration = false;
		}
		_framebuffers[0].unbind();
	}

	texture& gaussian_blur::test_get_texture() {
		return _textures[1];
	}

}

#endif