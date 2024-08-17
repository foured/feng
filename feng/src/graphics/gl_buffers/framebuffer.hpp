#pragma once

#include "i_gl_buffer.h"
#include "../shader.h"
#include "renderbuffer.hpp"
#include "arraybuffer.hpp"
#include "../vertexarray.h"

float screen_quad_vertices[] = {
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

namespace feng {

	class framebuffer : public i_gl_buffer{
	public:
		framebuffer(shader* fb_shader) 
			: _shader(fb_shader) {
			generate();
			bind();

			_vertexarray.generate();
			_vertexarray.bind();

			_vertexbuffer.generate();
			_vertexbuffer.bind();
			_vertexbuffer.buffer_data(sizeof(screen_quad_vertices), screen_quad_vertices, GL_STATIC_DRAW);

			_vertexarray.set_attrib_pointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
			_vertexarray.set_attrib_pointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (2 * sizeof(float)));

			glGenTextures(1, &_render_texture);
			glBindTexture(GL_TEXTURE_2D, _render_texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _render_texture, 0);

			_renderbuffer.generate();
			_renderbuffer.bind();
			_renderbuffer.renderbuffer_storage(GL_DEPTH24_STENCIL8, 800, 600);
			_renderbuffer.attach_to_framebuffer(GL_DEPTH_STENCIL_ATTACHMENT);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				LOG_ERROR("Framebuffer is not complete.");
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			_shader->activate();
			_shader->set_int("screenTexture", 0);
		}

		void render() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			_shader->activate();
			_vertexarray.bind();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _render_texture);
			_vertexarray.draw_arrays(GL_TRIANGLES, 6);
			vertexarray::unbind();
		}

		void delete_buffer() {
			glDeleteFramebuffers(1, &_FBO);
		}

		void set() {
			bind();
			glEnable(GL_DEPTH_TEST);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void generate() override {
			glGenFramebuffers(1, &_FBO);
		}

		void bind() override {
			glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
		}

		void unbind() override {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		uint32_t id() override {
			return _FBO;
		}

		int32_t type() override {
			return GL_FRAMEBUFFER;
		}

	private:
		shader* _shader;

		uint32_t _FBO, _render_texture;
		arraybuffer _vertexbuffer;
		renderbuffer _renderbuffer;
		vertexarray _vertexarray;
	};

}