#include "texture.h"

#include <stb/stb_image.h>

#include "../logging/logging.h"

namespace feng {

	texture::texture() { }

	texture::texture(std::string full_path, bool flip)
		: _type(aiTextureType::aiTextureType_NONE){
		generate();
		split_full_path(full_path);
		load_texture_from_file(flip);
	}

	texture::texture(std::string dir, std::string path, aiTextureType type) 
		: _type(type), _dir(dir), _path(path) {
		generate();
		load_texture_from_file();
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

			bind();
			set_params(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, param, param);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//glTexImage2D(GL_TEXTURE_2D, 0, color_mode, _width, _height, 0, color_mode, GL_UNSIGNED_BYTE, data);
			allocate(color_mode, _width, _height, GL_UNSIGNED_BYTE, data);
			generate_mipmap();
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

	void texture::allocate(uint32_t width, uint32_t height, uint32_t format, uint32_t type, uint8_t* data) {
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, data);
	}

	void texture::set_params(int32_t min_filter, int32_t mag_filter, int32_t wrap_s, int32_t wrap_t) {
		if(min_filter != NULL)
			set_param_i(GL_TEXTURE_MIN_FILTER, min_filter);
		if (mag_filter!= NULL)
			set_param_i(GL_TEXTURE_MAG_FILTER, mag_filter);
		if(wrap_s != NULL)
			set_param_i(GL_TEXTURE_WRAP_S, wrap_s);
		if(wrap_t != NULL)
			set_param_i(GL_TEXTURE_WRAP_T, wrap_t);
	}

	void texture::activate_slot(uint32_t slot) {
		glActiveTexture(GL_TEXTURE0 + slot);
	}

	void texture::generate_mipmap() {
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void texture::set_param_i(uint32_t pname, int32_t p) {
		glTexParameteri(GL_TEXTURE_2D, pname, p);
	}

	void texture::set_param_fv(uint32_t pname, const float* p) {
		glTexParameterfv(GL_TEXTURE_2D, pname, p);
	}

	int texture::id() const {
		return _id;
	}

	void texture::bind() {
		glBindTexture(GL_TEXTURE_2D, _id);
	}

	void texture::generate() {
		glGenTextures(1, &_id);
	}

	std::string texture::path() const {
		return _path;
	}

	aiTextureType texture::type() const {
		return _type;
	}

	int32_t texture::width() const {
		return _width;
	}

	int32_t texture::height() const {
		return _height;
	}

}