#include "texture.h"

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "../logging/logging.h"

namespace feng {

	texture::texture() {}

	texture::texture(std::string full_path, bool flip)
		: _type(aiTextureType::aiTextureType_NONE){
		generate_id();
		split_full_path(full_path);
		load_texture_from_file(flip);
	}

	texture::texture(std::string dir, std::string path, aiTextureType type) 
		: _type(type), _dir(dir), _path(path) {
		generate_id();
		load_texture_from_file();
	}

	void texture::generate_id() {
		glGenTextures(1, &_id);
	}

	void texture::load_texture_from_file(bool flip) {
		int32_t no_channels;
		stbi_set_flip_vertically_on_load(flip);
		uint8_t* data = stbi_load((_dir + "/" + _path).c_str(), &_width, &_height, &no_channels, 0);

		int param = GL_REPEAT;
		if (data) {
			uint32_t color_mode = GL_RGB;
			switch (no_channels)
			{
			case(1):
				color_mode = GL_RED;
				break;
			case(3):
				color_mode = GL_RGB;
				break;
			case(4):
				color_mode = GL_RGBA;
				param = GL_CLAMP_TO_EDGE;
				break;
			}

			glBindTexture(GL_TEXTURE_2D, _id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, color_mode, _width, _height, 0, color_mode, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			LOG_ERROR("Failed to load texture: '" + std::string(_dir + "/" + _path) + "'.");
		}
		stbi_image_free(data);
		LOG_ACTION("Loaded texture: '" + std::string(_dir + "/" + _path) + "'.");
	}

	void texture::split_full_path(std::string full_path) {
		int32_t lbs_pos = full_path.rfind('/');
		if (lbs_pos != std::string::npos) {
			_dir = full_path.substr(0, lbs_pos);
			_path = full_path.substr(_dir.size() + 1, full_path.size() - _dir.size());
		}
		else {
			LOG_ERROR("Error to split texture path: '" + full_path + "'.");
		}
	}

	int texture::id() {
		return _id;
	}

	void texture::bind() {
		glBindTexture(GL_TEXTURE_2D, _id);
	}

	std::string texture::path() {
		return _path;
	}

	aiTextureType texture::type() {
		return _type;
	}

	int32_t texture::width() {
		return _width;
	}

	int32_t texture::height() {
		return _height;
	}

}