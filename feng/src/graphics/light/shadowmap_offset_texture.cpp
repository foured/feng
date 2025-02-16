#include "shadowmap_offset_texture.h"

#include <random>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cassert>

#include <glad/glad.h>

float jitter() {
	static std::default_random_engine generator;
	static std::uniform_real_distribution<float> distrib(-0.5f, 0.5f);
	return distrib(generator);
}

namespace feng {

	shadowmap_offset_texture::shadowmap_offset_texture(uint32_t win_size, uint32_t filter_size) {
		std::vector<float> data;
		uint32_t buffer_size = win_size * win_size * filter_size * filter_size * 2;
		data.reserve(buffer_size);
		uint32_t idx = 0;

		for (int32_t tex_y = 0; tex_y < win_size; tex_y++) {
			for (int32_t tex_x = 0; tex_x < win_size; tex_x++) {
				for (int32_t v = filter_size - 1; v >= 0; v--) {
					for (int32_t u = 0; u < filter_size; u++) {
						float x = ((float)u + 0.5f + jitter()) / (float)filter_size;
						float y = ((float)v + 0.5f + jitter()) / (float)filter_size;

						assert(idx + 1 < data.size());
						data[idx] = sqrt(y) * cosf(2 * (float)M_PI * x);
						data[idx + 1] = sqrtf(y) * sinf(2 * (float)M_PI * x);
						idx += 2;
					}
				}
			}
		}

		uint32_t nfs = filter_size * filter_size;
		uint32_t hnfs = nfs / 2;
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_3D, _id);
		glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA32F, hnfs, hnfs, win_size);
		glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, hnfs, win_size, win_size, GL_RGBA, GL_FLOAT, &data[0]);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_3D, 0);
	}

	void shadowmap_offset_texture::bind() {
		glBindTexture(GL_TEXTURE_3D, _id);
	}

	void shadowmap_offset_texture::bind_to_slot(uint32_t slot) {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_3D, _id);
	}

}