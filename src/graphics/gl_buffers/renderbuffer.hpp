#pragma once

#include "i_gl_buffer.h"

namespace feng {

	class renderbuffer : public i_gl_buffer {
	public:
		void generate() override {
			glGenRenderbuffers(1, &_id);
		}

		void bind() override {
			glBindRenderbuffer(GL_RENDERBUFFER, _id);
		}

		void unbind() override {
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}

		uint32_t id() const override {
			return _id;
		}

		int32_t type() const override {
			return GL_RENDERBUFFER;
		}

		void renderbuffer_storage(uint32_t format, uint32_t width, uint32_t height) {
			glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
		}

		void attach_to_framebuffer(uint32_t attachment_mode) {
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment_mode, GL_RENDERBUFFER, _id);
		}

	private:
		uint32_t _id;
	};

}