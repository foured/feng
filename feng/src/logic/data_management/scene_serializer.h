#pragma once

#include <filesystem>
#include "../world/scene.h"

namespace feng::data {

	class scene_serializer {
	public:
		static void serialize(scene* scene, std::filesystem::path path);

	};

}