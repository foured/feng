#include "cubemap.h"

#include <stb/stb_image.h>

#include "../logging/logging.h"

namespace feng {

	cubemap::cubemap() {}

	cubemap::cubemap(uint32_t width, uint32_t height, uint32_t internalformat, uint32_t format, uint32_t type) {
		if (format == 0)
			format = internalformat;
		glGenTextures(1, &_texture_id);
		bind();
		for (uint32_t i = 0; i < 6; ++i) {
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalformat, width, height, 0, format, type, NULL);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	cubemap::cubemap(const std::array<std::string, 6>& faces) {
		glGenTextures(1, &_texture_id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _texture_id);

		int32_t width, height, n_channels;
		uint8_t* data;
		for (size_t i = 0; i < faces.size(); i++) {
			data = stbi_load(faces[i].c_str(), &width, &height, &n_channels, 0);
			if (data) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else {
				LOG_ERROR("Failed to load texture (cubemap): '" + faces[i] + "'.");
			}
			stbi_image_free(data);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		LOG_ACTION("Created cubemap texture: '" + faces[0] + "'.");
	}

	uint32_t cubemap::id() {
		return _texture_id;
	}

	void cubemap::bind() {
		glBindTexture(GL_TEXTURE_CUBE_MAP, _texture_id);
	}

}