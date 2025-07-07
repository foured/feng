#include "texture.h"

#ifdef _WIN32
	#define __STDC_LIB_EXT1__
#endif
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

	void texture::allocate(uint32_t width, uint32_t height, int32_t internalformat, void* data) {
		uint32_t format = 0;
		uint32_t type = 0;

		switch (internalformat) {
		case GL_R8:
			format = GL_RED;
			type = GL_UNSIGNED_BYTE;
			break;
		case GL_R32F:
			format = GL_RED;
			type = GL_FLOAT;
			break;
		case GL_R32UI:
			format = GL_RED_INTEGER;
			type = GL_UNSIGNED_INT;
			break;
		case GL_RG8:
			format = GL_RG;
			type = GL_UNSIGNED_BYTE;
			break;
		case GL_RGB8:
			format = GL_RGB;
			type = GL_UNSIGNED_BYTE;
			break;
		case GL_RGBA8:
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			break;
		case GL_RGBA32F:
			format = GL_RGBA;
			type = GL_FLOAT;
			break;
		case GL_DEPTH_COMPONENT24:
			format = GL_DEPTH_COMPONENT;
			type = GL_UNSIGNED_INT;
			break;
		case GL_DEPTH24_STENCIL8:
			format = GL_DEPTH_STENCIL;
			type = GL_UNSIGNED_INT_24_8;
			break;
		default:
			LOG_WARNING("Unsupported internal format: " + std::to_string(internalformat) + ". Using GL_RGBA and GL_UNSIGNED_BYTE as default.");
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			break;
		}

		allocate(width, height, internalformat, format, type, data);
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

		uint8_t noc = no_channels();
		uint8_t pixel_size = get_pixel_size();

		void* data = malloc(_width * _height * noc * pixel_size);
		glGetTexImage(GL_TEXTURE_2D, 0, _format, _type, data);
		GL_CHECK_ERRORS();
		return data;
	}

	int32_t texture::get_param_iv(uint32_t param) {
		int32_t val;
		glGetTexParameteriv(GL_TEXTURE_2D, param, &val);
		return val;
	}

	void texture::save_to_png(const std::string& path) {
		void* data = get_pixels();
		uint8_t noc = no_channels();
		if (stbi_write_png(path.c_str(), _width, _height, noc, data, _width * noc) == 0)
			LOG_WARNING("Error to save texture as PNG.");
		free(data);
		GL_CHECK_ERRORS();
	}

	texture_base_data texture::get_texture_data_form_file(const std::string path, bool flip) {
		int32_t width, height, no_channels;
		stbi_set_flip_vertically_on_load(flip);
		void* data = stbi_load(path.c_str(), &width, &height, &no_channels, 0);
		if(!data)
			LOG_ERROR("Failed to load texture: '" + path + "'.");
		return { data, width, height, no_channels };
	}

	uint32_t texture::format() const {
		return _format;
	}

	int32_t texture::internal_format() const {
		return _internalformat;
	}

	uint32_t texture::type() const {
		return _type;
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

	void texture::bind_to_slot(uint32_t slot) {
		texture::activate_slot(slot);
		bind();
	}

	void texture::generate() {
		glGenTextures(1, &_id);
	}

	void texture::delete_buffer() {
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
		file->write_raw(_type);
		uint8_t n = no_channels();
		file->write_raw(n);
		file->write_raw(_aiTtype);
		uint64_t no_pixeles = _width * _height * n * get_pixel_size();
		file->write_raw(no_pixeles);
		void* data = get_pixels();
		file->write_raw((char*)data, no_pixeles);
		free(data);
	}

	void texture::deserialize(data::rfile* file, scene* scene) {
		file->read_raw(&_width);
		file->read_raw(&_height);
		file->read_raw(&_type);
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
				break;
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

	uint8_t texture::get_pixel_size() const {
		switch (_type) {
		case GL_FLOAT:                  return 4;
		case GL_HALF_FLOAT:             return 2;
		case GL_INT:                    return 4;
		case GL_UNSIGNED_INT:           return 4;
		case GL_SHORT:                  return 2;
		case GL_UNSIGNED_SHORT:         return 2;
		case GL_BYTE:                   return 1;
		case GL_UNSIGNED_BYTE:          return 1;
		case GL_UNSIGNED_INT_24_8:      return 4;
		case GL_UNSIGNED_INT_10F_11F_11F_REV: return 4;
		case GL_UNSIGNED_INT_5_9_9_9_REV:     return 4;
		case GL_UNSIGNED_INT_2_10_10_10_REV:  return 4;
		case GL_UNSIGNED_INT_8_8_8_8:         return 4;
		case GL_UNSIGNED_SHORT_5_6_5:         return 2;
		case GL_UNSIGNED_SHORT_4_4_4_4:       return 2;
		case GL_UNSIGNED_SHORT_5_5_5_1:       return 2;
		default:
			LOG_WARNING("Unsupported texture type: " + std::to_string(_type) + " returning 1");
			return 1;
		}
	}

	uint8_t texture::no_channels() const {
		switch (_format)
		{
			// Single channel
		case GL_RED:
		case GL_RED_INTEGER:
		case GL_LUMINANCE:                  // Legacy
		case GL_ALPHA:                      // Legacy
		case GL_R8:
		case GL_R16:
		case GL_R16F:
		case GL_R32F:
		case GL_R8I:
		case GL_R8UI:
		case GL_R16I:
		case GL_R16UI:
		case GL_R32I:
		case GL_R32UI:
			return 1;

			// Dual channel
		case GL_RG:
		case GL_RG_INTEGER:
		case GL_LUMINANCE_ALPHA:            // Legacy
		case GL_RG8:
		case GL_RG16:
		case GL_RG16F:
		case GL_RG32F:
		case GL_RG8I:
		case GL_RG8UI:
		case GL_RG16I:
		case GL_RG16UI:
		case GL_RG32I:
		case GL_RG32UI:
			return 2;

			// Triple channel
		case GL_RGB:
		case GL_RGB_INTEGER:
		case GL_BGR:
		case GL_BGR_INTEGER:
		case GL_RGB8:
		case GL_RGB16:
		case GL_RGB16F:
		case GL_RGB32F:
		case GL_RGB8I:
		case GL_RGB8UI:
		case GL_RGB16I:
		case GL_RGB16UI:
		case GL_RGB32I:
		case GL_RGB32UI:
			return 3;

			// Quadruple channel
		case GL_RGBA:
		case GL_RGBA_INTEGER:
		case GL_BGRA:
		case GL_BGRA_INTEGER:
		case GL_RGBA8:
		case GL_RGBA16:
		case GL_RGBA16F:
		case GL_RGBA32F:
		case GL_RGBA8I:
		case GL_RGBA8UI:
		case GL_RGBA16I:
		case GL_RGBA16UI:
		case GL_RGBA32I:
		case GL_RGBA32UI:
			return 4;

			// Depth/stencil
		case GL_DEPTH_COMPONENT:
		case GL_DEPTH_COMPONENT16:
		case GL_DEPTH_COMPONENT24:
		case GL_DEPTH_COMPONENT32:
		case GL_DEPTH_COMPONENT32F:
			return 1;

		case GL_STENCIL_INDEX:
		case GL_STENCIL_INDEX1:
		case GL_STENCIL_INDEX4:
		case GL_STENCIL_INDEX8:
		case GL_STENCIL_INDEX16:
			return 1;

		case GL_DEPTH_STENCIL:
		case GL_DEPTH24_STENCIL8:
		case GL_DEPTH32F_STENCIL8:
			return 2;
		}
		LOG_WARNING("Unknown texture format detected (value: %d). Returning '4'.", _format);
		return 4;
	}

}