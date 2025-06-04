#pragma once

#include "i_gl_buffer.h"
#include "../shader.h"
#include "renderbuffer.hpp"
#include "arraybuffer.hpp"
#include "../vertexarray.h"
#include "../../logging/logging.h"
#include "../texture.h"

static const inline float screen_quad_vertices[] = {
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
		uint32_t width, height;


		framebuffer() {

		}

		framebuffer(uint32_t width, uint32_t height)
			: width(width), height(height){
			generate();
		}

		void attach_texture2d(const texture& tex, uint32_t attachment_mode) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_mode, GL_TEXTURE_2D, tex.id(), 0);
		}

		void attach_texture2d(uint32_t texture_id, uint32_t attachment_mode) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_mode, GL_TEXTURE_2D, texture_id, 0);
		}

		void attach_texture(const texture& tex, uint32_t attachment_mode) {
			glFramebufferTexture(GL_FRAMEBUFFER, attachment_mode, tex.id(), 0);
		}

		void attach_texture(uint32_t texture_id, uint32_t attachment_mode) {
			glFramebufferTexture(GL_FRAMEBUFFER, attachment_mode, texture_id, 0);
		}

		void attach_renderbuffer(const renderbuffer& rb, uint32_t attachment_mode) {
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment_mode, GL_RENDERBUFFER, rb.id());
		}

		texture allocate_and_attach_texture(uint32_t attachment_mode, uint32_t min_filter, uint32_t mag_filter, 
			uint32_t wrap_s, uint32_t wrap_t, int32_t internalformat, uint32_t format = NULL, uint32_t type = GL_FLOAT) {
			if (format == NULL)
				format = internalformat;
			texture fb_texture;
			fb_texture.generate();
			fb_texture.bind();
			fb_texture.allocate(width, height, internalformat, format,  type, NULL);
			fb_texture.set_params(min_filter, mag_filter, wrap_s, wrap_t);
			attach_texture2d(fb_texture, attachment_mode);

			return fb_texture;
		}

		renderbuffer allocate_and_attach_renderbuffer(uint32_t format, 
			uint32_t attachment_mode) {
			renderbuffer fb_renderbuffer;
			fb_renderbuffer.generate();
			fb_renderbuffer.bind();
			fb_renderbuffer.renderbuffer_storage(format, width, height);
			attach_renderbuffer(fb_renderbuffer, attachment_mode);

			return fb_renderbuffer;
		}

		static void check_status() {
			uint32_t dfb_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (dfb_status == GL_FRAMEBUFFER_UNDEFINED)
				LOG_ERROR("Framebuffer error: 'GL_FRAMEBUFFER_UNDEFINED'." );
			else if (dfb_status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
				LOG_ERROR("Framebuffer error: 'GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT'.");
			else if (dfb_status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
				LOG_ERROR("Framebuffer error: 'GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT'.");
			else if (dfb_status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
				LOG_ERROR("Framebuffer error: 'GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER'.");
			else if (dfb_status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
				LOG_ERROR("Framebuffer error: 'GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER'.");
			else if (dfb_status == GL_FRAMEBUFFER_UNSUPPORTED)
				LOG_ERROR("Framebuffer error: 'GL_FRAMEBUFFER_UNSUPPORTED'.");
			else if (dfb_status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)
				LOG_ERROR("Framebuffer error: 'GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE'.");
			else if (dfb_status == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS)
				LOG_ERROR("Framebuffer error: 'GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS'.");
		}
		
		void set_viewport() {
			glViewport(0, 0, width, height);
		}

		void set_draw_buffer(uint32_t mode) {
			glDrawBuffer(mode);
		}

		void set_draw_buffers(int32_t size, const uint32_t* data) {
			glDrawBuffers(size, data);
		}

		void set_read_buffer(uint32_t mode) {
			glReadBuffer(mode);
		}

		void delete_buffer() {
			glDeleteFramebuffers(1, &_FBO);
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

		uint32_t id() const override {
			return _FBO;
		}

		int32_t type() const override {
			return GL_FRAMEBUFFER;
		}

	private:
		uint32_t _FBO;

	};

}