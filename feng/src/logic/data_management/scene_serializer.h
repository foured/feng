#pragma once

#include <filesystem>

#include "../world/scene.h"

namespace feng::data {

	class scene_serializer {
	public:
		static void serialize(feng::scene* scene, std::filesystem::path path);
		static void serialize_models(feng::scene* scene, std::filesystem::path path);

		static void deserialize(feng::scene* scene, std::filesystem::path path);
		static void deserialize_models(feng::scene* scene, std::filesystem::path path);

	};

}