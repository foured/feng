#ifdef IMPLEMENT_WORK_IN_PROGRESS_CODE
#pragma once

#include <filesystem>

#include "../logic/world/scene.h"
#include "../logic/world/components/model_instance.h"

namespace feng::editor {

	class lightbaker {
	public:
		static void bake(scene* scene, const std::filesystem::path& path);

	private:
		using im_pair = std::pair<std::shared_ptr<instance>, std::shared_ptr<model_instance>>;

		static int32_t calculate_shadow_texture_res(const std::shared_ptr<model_instance>& model_instance);
	};

}
#endif