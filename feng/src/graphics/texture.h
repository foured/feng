#pragma once

#include <iostream>

#include <glad/glad.h>
#include <assimp/scene.h>

#include "../logic/data_management/serializable.h"

namespace feng {

	struct texture_base_data {
		void* data;
		int32_t width, height, no_channels;
	};

	class texture: public data::serializable {
	public:
		texture();
		texture(std::string full_path, bool flip = false);
		texture(std::string dir, std::string path, aiTextureType type = aiTextureType::aiTextureType_NONE);

		void generate();
		void bind();
		void allocate(uint32_t width, uint32_t height, int32_t internalformat, uint32_t format, uint32_t type = GL_FLOAT, void* data = NULL);
		void set_params(int32_t min_filter, int32_t mag_filter, int32_t wrap_s = NULL, int32_t wrap_t = NULL);
		void generate_mipmap();
		void del();

		void set_param_i(uint32_t pname, int32_t p);
		void set_param_fv(uint32_t pname, const float* p);

		void save_to_png(const std::string& path);
		void* get_pixels();
		void* get_pixels_safe();

		int32_t get_param_iv(uint32_t param);

		// Just 0, 1, 2... NO GL_TEXTURE0
		static void activate_slot(uint32_t slot);
		static texture_base_data get_texture_data_form_file(const std::string path, bool flip = false);
		static uint32_t get_current_binded_texture();

		int id() const;
		std::string path() const;
		aiTextureType aiTtype() const;
		int32_t width() const;
		int32_t height() const;
		uint8_t no_channels() const;
		uint32_t format() const;
		int32_t internal_format() const;
		uint32_t type() const;

		bool operator==(const texture& other) const {
			return _id == other._id;
		}

		void serialize(data::wfile* file) override;
		void deserialize(data::rfile* fil, scene* scene) override;

	private:
		uint32_t _id = 0;
		int32_t _width = 0, _height = 0;
		int32_t _internalformat = 0;
		uint32_t _format = 0;
		uint32_t _type = 0;

		aiTextureType _aiTtype = aiTextureType_NONE;
		std::string _path;
		std::string _dir;

		static uint32_t _current_binded_texture;

		void split_full_path(std::string full_path);
		void load_texture_from_file(bool flip = false);
	};

}