#pragma once

#include <iostream>

#include <glad/glad.h>
#include <assimp/scene.h>

namespace feng {

	class texture {
	public:
		texture();
		texture(std::string full_path, bool flip = false);
		texture(std::string dir, std::string path, aiTextureType type = aiTextureType::aiTextureType_NONE);

		void generate();
		void bind();
		void allocate(uint32_t width, uint32_t height, uint32_t format, uint32_t type = GL_FLOAT, uint8_t* data = NULL);
		void set_params(int32_t min_filter, int32_t mag_filter, int32_t wrap_s, int32_t wrap_t);
		void generate_mipmap();

		void set_param_i(uint32_t pname, int32_t p);
		void set_param_fv(uint32_t pname, const float* p);

		// Just 0, 1, 2... NO GL_TEXTURE0
		static void activate_slot(uint32_t slot);

		int id() const;
		std::string path() const;
		aiTextureType type() const;
		int32_t width() const;
		int32_t height() const;

		bool operator==(const texture& other) const {
			return _id == other._id;
		}

	private:
		uint32_t _id = 0;

		aiTextureType _type = aiTextureType_NONE;
		std::string _path;
		std::string _dir;

		int32_t _width = 0, _height = 0;

		void split_full_path(std::string full_path);
		void load_texture_from_file(bool flip = false);
	};

}