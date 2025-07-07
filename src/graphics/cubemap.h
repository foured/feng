#pragma once

#include <vector>
#include <string>
#include <array>

#include <glad/glad.h>

namespace feng {

	class cubemap {
	public:
		cubemap();
		cubemap(uint32_t width, uint32_t height, uint32_t internalformat, uint32_t format = 0, uint32_t type = GL_FLOAT);
		cubemap(const std::array<std::string, 6>& faces);

		uint32_t id();
		void bind();

	private:
		uint32_t _texture_id;
	};

}