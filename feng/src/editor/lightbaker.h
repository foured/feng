#pragma once

#include <filesystem>

#include "../logic/world/scene.h"

namespace feng::editor {

	class lightbaker {
		static void bake(scene* scene, const std::filesystem::path& path);
	};

}