#pragma once

#include <iostream>

#include <assimp/scene.h>

namespace feng {

	class texture {
	public:
		texture();
		texture(std::string full_path, bool flip = false);
		texture(std::string dir, std::string path, aiTextureType type = aiTextureType::aiTextureType_NONE);

		int id();
		void bind();
		std::string path();
		aiTextureType type();
		int32_t width();
		int32_t height();

		bool operator==(const texture& other) const {
			return _id == other._id;
		}

	private:
		uint32_t _id;

		aiTextureType _type;
		std::string _path;
		std::string _dir;

		int32_t _width, _height;

		void split_full_path(std::string full_path);
		void generate_id();
		void load_texture_from_file(bool flip = false);
	};

}