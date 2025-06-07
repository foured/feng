#include "texture.h"

#define __STDC_LIB_EXT1__
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include "../logging/logging.h"
#include "../logic/data_management/files.h"
#include "../logic/world/scene.h"

namespace feng {

	texture::texture() { }

	texture::texture(std::string full_path, bool flip)
		: _aiTtype(aiTextureType::aiTextureType_NONE){
		generate();
		split_full_path(full_path);
		load_texture_from_file(flip);
	}

	texture::texture(std::string dir, std::string path, aiTextureType type) 
		: _aiTtype(type), _dir(dir), _path(path) {
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
			allocate(_width, _height, color_mode, color_mode, GL_UNSIGNED_BYTE, data);
			set_params(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, param, param);
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

	void texture::allocate(uint32_t width, uint32_t height, int32_t internalformat, uint32_t format, uint32_t type, void* data) {
		_width = width;
		_height = height;
		_internalformat = internalformat;
		_format = format;
		_type = type;
		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, data);
	}

	void texture::set_params(int32_t min_filter, int32_t mag_filter, int32_t wrap_s, int32_t wrap_t) {
		if(min_filter != NULL)
			set_param_i(GL_TEXTURE_MIN_FILTER, min_filter);
		if(mag_filter != NULL)
			set_param_i(GL_TEXTURE_MAG_FILTER, mag_filter);
		if(wrap_s != NULL)
			set_param_i(GL_TEXTURE_WRAP_S, wrap_s);
		if(wrap_t != NULL)
			set_param_i(GL_TEXTURE_WRAP_T, wrap_t);
	}

	void* texture::get_pixels() {
		bind();
		int noc = no_channels();
		void* data = malloc(_width * _height * noc);
		glGetTexImage(GL_TEXTURE_2D, 0, _format, _type, data);
		GL_CHECK_ERRORS();
		return data;
	}

	void* texture::get_pixels_safe() {
		bind();

		const int level = 0;
		const int noc = no_channels(); 
		const int pixel_size = _width * _height * noc;

		void* data = malloc(pixel_size);
		if (!data) {
			LOG_ERROR("Failed to allocate memory for texture read.");
			return nullptr;
		}

		glGetTexImage(GL_TEXTURE_2D, level, _format, _type, data);
		GLenum err = glGetError();
		if (err == GL_NO_ERROR) {
			return data;
		}

		LOG_WARNING("glGetTexImage failed; falling back to RGBA.");
		free(data);
		data = nullptr;

		const int fallback_noc = 4;
		const int fallback_size = _width * _height * fallback_noc;

		data = malloc(fallback_size);
		if (!data) {
			LOG_ERROR("Failed to allocate memory for RGBA fallback.");
			return nullptr;
		}

		glGetTexImage(GL_TEXTURE_2D, level, GL_RGBA, GL_UNSIGNED_BYTE, data);
		err = glGetError();
		if (err != GL_NO_ERROR) {
			LOG_ERROR("glGetTexImage(GL_RGBA) failed.");
			free(data);
			return nullptr;
		}

		if (noc == 1) {
			void* red_only = malloc(pixel_size);
			if (!red_only) {
				LOG_ERROR("Failed to allocate red_only buffer.");
				free(data);
				return nullptr;
			}

			uint8_t* src = static_cast<uint8_t*>(data);
			uint8_t* dst = static_cast<uint8_t*>(red_only);
			for (int i = 0; i < _width * _height; ++i) {
				dst[i] = src[i * 4]; // только R
			}

			free(data);
			return red_only;
		}

		return data;
	}


	void texture::save_to_png(const std::string& path) {
		void* data = get_pixels();
		uint8_t noc = no_channels();
		if (stbi_write_png(path.c_str(), _width, _height, noc, data, _width * noc) == 0)
			LOG_WARNING("Error to save texture as PNG.");
		free(data);
	}

	texture_base_data texture::get_texture_data_form_file(const std::string path, bool flip) {
		int32_t width, height, no_channels;
		stbi_set_flip_vertically_on_load(flip);
		void* data = stbi_load(path.c_str(), &width, &height, &no_channels, 0);
		if(!data)
			LOG_ERROR("Failed to load texture: '" + path + "'.");
		return { data, width, height, no_channels };
	}

	uint8_t texture::no_channels() const{
		switch (_format)
		{
		case (GL_RED):
			return 1;
		case (GL_RG):
			return 2;
		case (GL_RGB):
			return 3;
		case (GL_RGBA):
			return 4;
		case (GL_DEPTH_COMPONENT):
			return 4;
		}
		LOG_WARNING("Unknown texture format detected. Returning '4'.");
		return 4;
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

	void texture::del() {
		glDeleteTextures(1, &_id);
	}

	std::string texture::path() const {
		return _path;
	}

	aiTextureType texture::aiTtype() const {
		return _aiTtype;
	}

	int32_t texture::width() const {
		return _width;
	}

	int32_t texture::height() const {
		return _height;
	}

	void texture::serialize(data::wfile* file) {
		file->write_raw(_width);
		file->write_raw(_height);
		uint8_t n = no_channels();
		file->write_raw(n);
		file->write_raw(_aiTtype);
		uint64_t no_pixeles = _width * _height * n;
		file->write_raw(no_pixeles);
		void* data = get_pixels_safe();
		file->write_raw((char*)data, no_pixeles);
		free(data);
	}

	void texture::deserialize(data::rfile* file, scene* scene) {
		file->read_raw(&_width);
		file->read_raw(&_height);
		uint8_t n;
		file->read_raw(&n);
		file->read_raw(&_aiTtype);
		uint64_t no_pixeles;
		file->read_raw(&no_pixeles);
		void* data = malloc(no_pixeles);
		file->read_raw((char*)data, no_pixeles);
		file->check_stream();

		if (data) {
			generate();
			uint32_t color_mode = GL_RGB;
			int32_t param = GL_REPEAT;
			switch (n)
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
			}

			bind();
			allocate(_width, _height, color_mode, color_mode, GL_UNSIGNED_BYTE, data);
			set_params(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, param, param);
			generate_mipmap();
			LOG_ACTION("Texture loaded.");
		}
		else {
			LOG_ERROR("Failed to load texture.");
		}
		free(data);
	}

}